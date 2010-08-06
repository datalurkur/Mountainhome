require 'Terrain'

class TerrainBuilder
    # Generates a new heightmap and layers it *on top* of any existing terrain
    def self.add_layer(terrain, type, offset=0.0, scale=1.0, entropy=10.0, granularity=0.4)
        $logger.info("Adding new layer : type #{type} scale #{scale} entropy #{entropy} granularity #{granularity}")
        offset = [offset, 1.0-scale].min

        midpoint_layer = HeightMap.midpoint(terrain.width, entropy, granularity)
        voronois_layer = HeightMap.voronois(terrain.width, 10)
        rough_layer = HeightMap.mix(terrain.width, [midpoint_layer, voronois_layer], [0.5, 0.5])

        layer = HeightMap.scale(1+(offset*(terrain.depth-1)), scale*(terrain.depth-1), rough_layer)
        
        layer.each_with_index do |row, x|
            row.each_with_index do |col, y|
                offset = 1 + terrain.get_surface(x, y)
                range = (0..col.floor)
                range.each { |z| terrain.set_tile(x, y, (z + offset), type) if ((z+offset) < terrain.depth)}
            end
        end

        terrain
    end

    # Generates a new heightmap and merges it with existing terrain
    # In this case, any existing terrain is left alone and only areas where the new heightmap rises
    #  above the existing terrain is any terrain data added
    def self.composite_layer(terrain, type, offset = 0.0, scale=1.0, entropy=10.0, granularity=0.4)
        $logger.info("Compositing layers [#{type}]: scale #{scale} entropy #{entropy} granularity #{granularity}")
        offset = [offset, 1.0-scale].min
        
        midpoint_layer = HeightMap.midpoint(terrain.width, entropy, granularity)
        voronois_layer = HeightMap.voronois(terrain.width, 10)
        rough_layer = HeightMap.mix(terrain.width, [midpoint_layer, voronois_layer], [0.5, 0.5])

        layer = HeightMap.scale(1+(offset*(terrain.depth-1)), scale*(terrain.depth-1), rough_layer)
        
        layer.each_with_index do |row, x|
            row.each_with_index do |col, y|
                offset = 1 + terrain.get_surface(x, y)
                range = (offset..col.floor)
                range.each { |z| terrain.set_tile(x, y, z, type) if (z < terrain.depth)}
            end
        end
        
        terrain
    end

    def self.shear(terrain, magnitude=5, variation=1, cap)
        $logger.info "Shearing terrain: magnitude #{magnitude} variation #{variation} cap #{cap}"

        # Determine which way the fault will run and which side of the fault will be lowered
        # If start_axis is :x, we'll begin somewhere *on* the X-axis
        start_axis  = (rand(2).to_i == 0) ? :x   : :y
        # :low means we lower [0...point], :high means we lower [point...boundary]
        raised_side = (rand(2).to_i == 0) ? :low : :high

        # Start at a random location at one of the lower edges of the map
        # Initialize our direction vector to point us towards the opposite edge
        case start_axis
            when :x
                start_position = [rand(terrain.width),  0]
                direction      = [0,                    1]
            when :y
                start_position = [0, rand(terrain.height)]
                direction      = [1,                    0]
        end

        current_position = start_position

        # Move towards the opposite edge one unit at a time until we get there or go out of bounds
        until terrain.out_of_bounds?(current_position[0], current_position[1], 0)
            # STEP 1:
            # Take a slice of the map between our current position and the appropriate edge
            #  as dictated by raised_side and drop it down some number of units equal to the
            #  magnitude of the fault

            # The midpoint of this slice is dependent on our axis of movement
            slice_median = ((start_axis == :x) ? current_position[0] : current_position[1])

            # The range of the slice is dependent on both our axis of movement and the raised side
            slice_range = case raised_side
                when :low
                    (0..slice_median)
                when :high
                    (start_axis == :x) ? (slice_median...terrain.width) : (slice_median...terrain.height)
            end

            # Iterate over each element in the slice and drop its column by the shear magnitude
            slice_range.each do |slice_elem|
                # Generate the coordinates for this element of the slice orthogonal to our axis of movement
                slice_coords = (start_axis == :x) ? [slice_elem, current_position[1]] : [current_position[0], slice_elem]

                # Drop the column
                drop_column(terrain, slice_coords[0], slice_coords[1], magnitude)
            end

            # STEP 2:
            # Generate a random directional variation in the range [-variation, variation]
            direction_variation = (rand((variation*2)+1) - variation).to_i

            # Add the directional variation to our current direction vector
            case start_axis
                when :x
                    direction[0] += direction_variation
                when :y
                    direction[1] += direction_variation
            end

            # Check to make sure the directional vector doesn't exceed the cap if it's set
            if cap
                direction.each_with_index do |d,i|
                    direction[i] = [[d,-cap].max, cap].min
                end
            end

            # STEP 3:
            # Advance the fault along the direction vector
            current_position[0] += direction[0]
            current_position[1] += direction[1]

            # As a corner case, we need to make sure the fault makes it all the way across the map,
            #  even if it travels along the edge.  This way, the fault doesn't end early and leave a
            #  large portion of the map raised if it hits the raised edge early.
            case start_axis
                when :x
                    current_position[0] = [[current_position[0], 0].max,  terrain.width-1].min
                when :y
                    current_position[1] = [[current_position[1], 0].max, terrain.height-1].min
            end
        end
    end

    def self.average(terrain, passes = 1)
        $logger.info "Averaging terrain: passes #{passes}"
        coords = []

        (0...terrain.width).each do |x|
            (0...terrain.height).each do |y|
                coords << [x,y]
            end
        end
            
        passes.times do
            coords.sort { rand(2)-1 }
            coords.each do |x, y|
                thisVal = terrain.get_surface(x, y)

                # stop if there is no tile at x,y
                next if thisVal == -1

                thisType = terrain.get_tile(x, y, thisVal)
                
                vals = []
                neighbors = [[x-1,y+1,1],[x,y+1,2],[x+1,y+1,1],
                             [x-1,y  ,2],          [x+1,y  ,2],
                             [x-1,y-1,1],[x,y-1,2],[x+1,y-1,1]]

                neighbors.each do |neighbor|
                    if !terrain.out_of_bounds?(neighbor[0], neighbor[1], 0)
                        neighbor[2].times { vals << terrain.get_surface(neighbor[0], neighbor[1]) }
                    end
                end

                # newVal = vals.inject { |sum,val| sum ? sum+val : val } / vals.size
                newVal = vals.inject(0, &:+) / vals.size

                while newVal != thisVal
                    if newVal > thisVal
                        #$logger.info "Averaging tile upwards"
                        terrain.set_tile(x, y, thisVal+1, thisType)
                        thisVal += 1
                    else
                        #$logger.info "Averaging tile downwards"
                        terrain.set_tile(x, y, thisVal, 0)
                        thisVal -= 1
                    end
                end
            end
        end
    end

    def self.generate_riverbeds(terrain, num_sources=1)
        $logger.info "Generating riverbeds"

        # Generate seed points to build rivers off of
        seed_points = []
        num_sources.times { seed_points << [rand(terrain.width),rand(terrain.height)] }

        # For each seed point, path both upwards and downwards
        river_points = []
        seed_points.each do |origin|
            $logger.info "Computing path for #{origin.inspect}"

            # Path downards to the edge or ocean
            position  = origin[0..1]
            path = []
            next_height = terrain.get_surface(position[0], position[1])
            begin
                height = next_height

                x_p = position[0]-1
                x_c = position[0]
                x_n = position[0]+1
                y_p = position[1]-1
                y_c = position[1]
                y_n = position[1]+1

                neighbors = [[x_p, y_n],[x_c, y_n],[x_n, y_n],
                             [x_p, y_c],           [x_n, y_c],
                             [x_p, y_p],[x_c, y_p],[x_n, y_p]]

                # Kill off any neighbors that are already in the path
                neighbors = neighbors.select { |n| !path.include?(n) }

                break if neighbors.size == 0

                # Add the current position to the end of our path
                path << [position[0],position[1]]

                # Get the neighboring heights, and keep them packed with their neighbor index
                heights = []
                neighbors.each_with_index do |n,i|
                    heights << [terrain.get_surface(n[0],n[1]), i]
                end
                # Sort the heights
                heights.sort! { |x,y| x[0] <=> y[0] }

                # Move towards the lowest height
                position[0] = neighbors[heights.first[1]][0]
                position[1] = neighbors[heights.first[1]][1]

                # Have we gone out of bounds?
                break if terrain.out_of_bounds?(position[0], position[1], 0)

                # Next!
                next_height = terrain.get_surface(position[0], position[1])
            end while (next_height <= height)

            # Path upwards to the local maxima or edge
            position = origin[0..1]
            next_height = terrain.get_surface(position[0], position[1])
            begin
                height = next_height

                x_p = position[0]-1
                x_c = position[0]
                x_n = position[0]+1
                y_p = position[1]-1
                y_c = position[1]
                y_n = position[1]+1

                neighbors = [[x_p, y_n],[x_c, y_n],[x_n, y_n],
                             [x_p, y_c],           [x_n, y_c],
                             [x_p, y_p],[x_c, y_p],[x_n, y_p]]

                # Kill off any neighbors that are already in the path
                neighbors = neighbors.select { |n| !path.include?(n) }

                break if neighbors.size == 0

                # Add the current position to the beginning of our path
                path = [[position[0],position[1]]] + path

                # Get the neighboring heights, and keep them packed with their neighbor index
                heights = []
                neighbors.each_with_index do |n,i|
                    heights << [terrain.get_surface(n[0],n[1]), i]
                end
                # Sort the heights
                heights.sort! { |x,y| x[0] <=> y[0] }

                # Move towards the highest height
                position[0] = neighbors[heights.last[1]][0]
                position[1] = neighbors[heights.last[1]][1]

                # Have we gone out of bounds?
                break if terrain.out_of_bounds?(position[0], position[1], 0)

                # Next!
                next_height = terrain.get_surface(position[0], position[1])
            end while (next_height >= height)

            $logger.info "Done, adding path to rivers"
            path.each do |point|
                if river_points.include?(point)
                    river_points[river_points.index(point)][2] += 1
                else
                    river_points << (point << 1)
                end
            end
        end

        # Iterate over the river points, queueing up points to be eroded
        erode_points = []
        river_points.each do |point|
            # Erode the landscape at each point the river passes through using a tapered window
            # This has the effect of rolling a (rough) sphere across the landscape, making a dent as it rolls
            # The size of our window is determined by the breadth of the river (how many river seeds pass through this point)
            breadth = point[2]
            b_range = (-breadth..breadth)

            # Iterate over each point in the window, checking to see if the point in question is already scheduled to be eroded
            b_range.each do |x|
                b_range.each do |y|
                    erode_x = point[0] + x
                    erode_y = point[1] + y

                    # Make sure this point is within the boundaries
                    next if terrain.out_of_bounds?(erode_x, erode_y, 0)

                    erode_depth = ((4*(breadth**2)) / ((x**2)+(y**2)+(2*(breadth**2)))).to_i + 1

                    # Is this point already set to be eroded?
                    preexisting = erode_points.select { |e_p| ((e_p[0] == erode_x) && (e_p[1] == erode_y)) }
                    if (preexisting.size > 0)
                        # This point already exists
                        p_index = erode_points.index(preexisting.first)
                        # Of the two depths in question (the previous specified depth and this depth), keep the deepest
                        erode_points[p_index][2] = [erode_points[p_index][2], erode_depth].max
                    else
                        # Add this point to the list of points to be eroded
                        erode_points << [erode_x, erode_y, erode_depth]
                    end
                end
            end
        end

        # Erode the points previously specified
        erode_points.each do |point|
            drop_column(terrain, point[0], point[1], point[2])
        end
    end

    def self.fill_ocean(terrain, liquid)
        # Take an average of the landscape height
        average_height = 0
        (0...terrain.width).each do |x|
            (0...terrain.height).each do |y|
                average_height += terrain.get_surface(x,y)
            end
        end
        average_height /= (terrain.width * terrain.height)
        $logger.info "Filling oceans to depth #{average_height}"

        # Fill water to this level
        # TODO - Add some variation here to make this builder step more sophisticated and interesting
        (0...terrain.width).each do |x|
            (0...terrain.height).each do |y|
                surface_level = terrain.get_surface(x,y)
                ((surface_level+1)..average_height).each do |z|
                    # $logger.info "Adding liquid node at #{[x,y,z].inspect}"
                    liquid.set_liquid(x,y,z,2,1.0)
                end
            end
        end
    end

    # =================
    # Utility functions
    # =================

    # Lower a single column of tiles at x,y by amount z-levels
    def self.drop_column(terrain,x,y,amount)
        s_level = terrain.get_surface(x, y)
        offset = [s_level, amount].min
        (0..s_level).each do |z_level|
            new_type = (((z_level + offset) < terrain.depth) ? terrain.get_tile(x, y, z_level + offset) : 0)
            terrain.set_tile(x, y, z_level, new_type)
        end
    end

    # This is just a unit test, basically
    def self.test_populate(terrain)
        seed = rand(100000)
        srand(seed)
        num_points = 1000
        value_range = 1000

        $logger.info "Perfoming sanity check on terrain with seed #{seed}"

        # Generate a whole bunch of random test points
        test_points = []
        test_values = []
        $logger.info "=Generating test points"
        num_points.times do
            new_pt  = [rand(terrain.width), rand(terrain.height), rand(terrain.depth)]
            new_val = rand(value_range)
            if not test_points.include? new_pt
                test_points << new_pt
                test_values << new_val
            end
        end

        # Set values
        $logger.info "=Seeding test points"
        test_points.each_with_index { |pt,ind| terrain.set_tile(pt[0], pt[1], pt[2], test_values[ind]) }

        # Verify that values emerge the same as when they went in
        $logger.info "=Verifying test points..."
        test_points.each_with_index do |pt, ind|
            ret_val = terrain.get_tile(pt[0], pt[1], pt[2])
            if ret_val != test_values[ind]
                $logger.info "****FAILURE in terrain test for point #{pt}"
            end
        end
        $logger.info "...Done!"
    end
end

class HeightMap
    def self.scale(min, max, opt_array=nil)
        array = opt_array || @array
        arr_min = array.collect(&:min).min
        arr_max = array.collect(&:max).max

        $logger.info "Scaling heightmap #{[arr_min, arr_max].inspect} to #{[min, max].inspect}"

        scalar = max / (arr_max - arr_min)

        array.each_with_index do |row, x|
            array[x].collect! { |c| (((c  - arr_min) * scalar) + min) }
        end

        $logger.info "Finished scaling heightmap."

        array
    end

    def self.distance(source, dest)
        return ((source[0] - dest[0])**2 + (source[1] - dest[1])**2)**0.5
    end

    def self.mix(size, maps, coeff)
        if maps.size != coeff.size
            return maps.first
        end

        mixed = Array.new(size) { Array.new(size,0) }
        maps.each_with_index do |map,i|
            size.times do |x|
                size.times do |y|
                    mixed[x][y] += (map[x][y]*coeff[i])
                end
            end
        end
        mixed
    end

    def self.voronois(size, features, n=2, coeff=[-1,1])
        # Initialize the grid
        @array = Array.new(size) { Array.new(size,0) }

        # Check the arguments for sanity
        if n > features || coeff.size != n
            $logger.info "Bad arguments passed to voronois generation"
            return @array
        end

        # Generate feature points
        feature_points = []
        features.times { feature_points << [rand(size), rand(size)] }

        # Iterate across the diagram, setting the magnitude of each point
        #  according to its distance from the features
        size.times do |x|
            size.times do |y|
                # Compute the distance to each feature
                feat_dist = feature_points.collect { |feature| HeightMap.distance(feature, [x,y]) }
                # Sort the features by distance
                feat_dist.sort!

                # Compute the value at this location using the closest n features
                h_value = 0
                (0...n).each do |index|
                    h_value += (feat_dist[index] * coeff[index])
                end

                # Set the value
                @array[x][y] = h_value
            end
        end

        # Return the array
        @array
    end

    def self.midpoint(size, localEntropy, granularity, level=2)
        $logger.info "Generating height map: size #{size} localEntropy #{localEntropy} granularity #{granularity} level #{level}"
        @array = Array.new(size) { Array.new(size,0) }
        
        if level + 1 < size
            midpoint(size, localEntropy / granularity, granularity, level*2)
        else
            # Instantiate an array of random start values
            iVals=Array.new(4)
            iRange = localEntropy / granularity
            iVals.collect! { |x| rand(iRange)-(iRange/2.0) }

            # Seed the terrain
            @array[0][0]           = iVals[0]
            @array[0][size-1]      = iVals[1]
            @array[size-1][0]      = iVals[2]
            @array[size-1][size-1] = iVals[3]
        end

        # The highest level will be computed first, with 1 segment
        segments = (size - 1) / level

        # First, compute the midpoints of all the sections
        segments.times do |x|
            segments.times do |y|
                # Divide the grid into segments ** 2 pieces
                calcBox(x*level, y*level, (x+1)*level, (y+1)*level, localEntropy)
            end
        end

        # Then, use the midpoint calculations to compute the halfway heights for edges
        segments.times do |x|
            segments.times do |y|
                calcDiamond(x*level, y*level, (x+1)*level, (y+1)*level, size, localEntropy)
            end
        end

        @array
    end

    # The first step in midpoint heightmapping
    def self.calcBox(lowerX, lowerY, upperX, upperY, localEntropy)
        # Calculate our halfway points
        offset = (upperX - lowerX) / 2.0
        middleX = lowerX + offset
        middleY = lowerY + offset

        # Compute the size of intersection of the diagonals
        # as the average of the four corners plus a random amount
        middle = @array[lowerX][lowerY] + @array[lowerX][upperY] + @array[upperX][lowerY] + @array[upperX][upperY]
        middle /= 4.0
        middle += (rand(localEntropy) - (localEntropy/2.0))

        @array[middleX][middleY] = middle
    end

    # The second step in midpoint heightmapping
    def self.calcDiamond(lowerX, lowerY, upperX, upperY, size, localEntropy)
        # Compute indices
        offset = (upperX - lowerX) / 2.0
        middleX = lowerX + offset
        middleY = lowerY + offset
        preX = lowerX - offset
        postX = upperX + offset
        preY = lowerY - offset
        postY = upperY + offset
        
        midLeft = midRight = midTop = midBottom = @array[middleX][middleY]

        midLeft += @array[lowerX][lowerY] + @array[lowerX][upperY]
        (preX < 0) ? midLeft *= 4.0 / 3.0 : midLeft += @array[preX][middleY]
        midLeft /= 4.0
        midLeft += (rand(localEntropy) - (localEntropy/2.0))
        
        midRight += @array[upperX][lowerY] + @array[upperX][upperY]
        (postX >= size) ? midRight *= 4.0 / 3.0 : midRight += @array[postX][middleY]
        midRight /= 4.0
        midRight += (rand(localEntropy) - (localEntropy/2.0))
        
        midTop += @array[lowerX][upperY] + @array[upperX][upperY]
        (postY >= size) ? midTop *= 4.0 / 3.0 : midTop += @array[middleX][postY]
        midTop /= 4.0
        midTop += (rand(localEntropy) - (localEntropy/2.0))
        
        midBottom += @array[lowerX][lowerY] + @array[upperX][lowerY]
        (preY < 0) ? midBottom *= 4.0 / 3.0 : midBottom += @array[middleX][preY]
        midBottom /= 4.0
        midBottom += (rand(localEntropy) - (localEntropy/2.0))
        
        @array[lowerX][middleY] = midLeft
        @array[upperX][middleY] = midRight
        @array[middleX][upperY] = midTop
        @array[middleX][lowerY] = midBottom
    end
end
