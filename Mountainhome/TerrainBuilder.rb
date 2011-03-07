class Array
    def normalize
        magnitude = (self.inject(0) { |sum,i| sum + (i**2) }) ** 0.5
        self.collect { |i| i / magnitude }
    end
end

class TerrainBuilder
    def self.form_tunnel(world, tunnel_size=3, tunnel_wander=0.4, irregularity=0.5, damping=0.4, damping_power=2, type=nil)
        # Roll random vectors to determine starting directions and position
        position  = [rand(world.width), rand(world.height)]
        surface = world.terrain.get_surface(*position)
        position << rand(surface)

        direction = [rand-0.5, rand-0.5, rand-0.5]
        direction = direction.normalize

        # Traverse in both directions away from the starting position
        [1, -1].each do |direction_mod|
            damping_factor = 1.0
            dir = direction.collect { |i| i * direction_mod }
            pos = position.dup

            until (brush_size = tunnel_size * damping_factor) <= 0.5
                # Compute the spherical brush and reject any tiles that are out of bounds
                brush = compute_spherical_batch(brush_size, irregularity, pos)
                brush.reject! { |i| world.out_of_bounds?(*i) }

                # Batch set the brush to the specified tunnel tile (empty, in this case)
                if type.nil?
                    batch_set_empty(world, brush)
                else
                    batch_set_material(world, type, brush)
                end

                # Move the position along the directional axis
                pos.each_index { |i| pos[i] = pos[i] + dir[i] }
                break if world.out_of_bounds?(*pos)

                # Modify the direction
                dir.each_index { |i| dir[i] += ((rand - 0.5) * tunnel_wander) }
                dir = dir.normalize

                # Shrink the size of the tunnel by some damping factor
                damping_factor *= 1.0 - ((rand * damping) ** damping_power)
            end
        end
    end

    # ========================
    # TERRAIN ADDITION METHODS
    # ========================

    # Generates a new heightmap and layers it *on top* of any existing terrain
    def self.add_layer(world, type_klass, offset=0.0, scale=1.0, entropy=10.0, granularity=0.4)
        type = type_klass.class_attributes[:material_id]
        $logger.info("Adding new layer : type #{type} scale #{scale} entropy #{entropy} granularity #{granularity}")

        offset = [offset, 1.0-scale].min

        midpoint_layer = HeightMap.midpoint(world.width, entropy, granularity)
        midpoint_layer = HeightMap.scale(1+(offset*(world.depth-1)), scale*(world.depth-1), midpoint_layer)

        voronois_layer = HeightMap.voronois(world.width)
        voronois_layer = HeightMap.scale(1+(offset*(world.depth-1)), scale*(world.depth-1), voronois_layer)

        layer = HeightMap.mix(world.width, [midpoint_layer, voronois_layer], [0.6, 0.4])

        layer.each_with_index do |row, x|
            row.each_with_index do |col, y|
                offset = 1 + world.get_surface(x, y)
                range = (0..col.floor)
                range.each { |z| world.set_tile_material(x, y, (z + offset), type) if ((z+offset) < world.depth)}
            end
        end

        world
    end

    # Generates a new heightmap and merges it with existing terrain
    # In this case, any existing terrain is left alone and only areas where the new heightmap rises
    #  above the existing terrain is any terrain data added
    def self.composite_layer(world, type_klass, offset = 0.0, scale=1.0, entropy=10.0, granularity=0.4)
        type = type_klass.class_attributes[:material_id]
        $logger.info("Compositing layers [#{type}]: scale #{scale} entropy #{entropy} granularity #{granularity}")
        offset = [offset, 1.0-scale].min

        midpoint_layer = HeightMap.midpoint(world.width, entropy, granularity)
        midpoint_layer = HeightMap.scale(1+(offset*(world.depth-1)), scale*(world.depth-1), midpoint_layer)

        voronois_layer = HeightMap.voronois(world.width)
        voronois_layer = HeightMap.scale(1+(offset*(world.depth-1)), scale*(world.depth-1), voronois_layer)

        layer = HeightMap.mix(world.width, [midpoint_layer, voronois_layer], [0.6, 0.4])

        layer.each_with_index do |row, x|
            row.each_with_index do |col, y|
                offset = 1 + world.get_surface(x, y)
                range = (offset..col.floor)
                range.each { |z| world.set_tile_material(x, y, z, type) if (z < world.depth)}
            end
        end

        world
    end

    def self.shear(world, magnitude=5, variation=1, cap)
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
                start_position = [rand(world.width),  0]
                direction      = [0,                    1]
            when :y
                start_position = [0, rand(world.height)]
                direction      = [1,                    0]
        end

        current_position = start_position

        # Move towards the opposite edge one unit at a time until we get there or go out of bounds
        until world.out_of_bounds?(current_position[0], current_position[1], 0)
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
                    (start_axis == :x) ? (slice_median...world.width) : (slice_median...world.height)
            end

            # Iterate over each element in the slice and drop its column by the shear magnitude
            slice_range.each do |slice_elem|
                # Generate the coordinates for this element of the slice orthogonal to our axis of movement
                slice_coords = (start_axis == :x) ? [slice_elem, current_position[1]] : [current_position[0], slice_elem]

                # Drop the column
                drop_column(world, slice_coords[0], slice_coords[1], magnitude)
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
                    current_position[0] = [[current_position[0], 0].max,  world.width-1].min
                when :y
                    current_position[1] = [[current_position[1], 0].max, world.height-1].min
            end
        end
    end

    def self.average(world, passes = 1)
        $logger.info "Averaging terrain: passes #{passes}"
        coords = []

        (0...world.width).each do |x|
            (0...world.height).each do |y|
                coords << [x,y]
            end
        end

        passes.times do
            coords.sort_by { rand }
            coords.each do |x, y|
                #$logger.info "Getting surface for #{[x,y].inspect}"
                thisVal  = world.get_surface(x, y)
                next if thisVal < 0 || thisVal >= world.depth
                thisType = world.get_tile_material(x, y, thisVal)

                vals = []
                window_width = 3
                (x-window_width..x+window_width).each do |local_x|
                    (y-window_width..y+window_width).each do |local_y|
                        next if world.out_of_bounds?(local_x, local_y, 0)
                        radius = (((local_x - x)**2 + (local_y - y)**2)**0.5).to_i
                        radius.times { vals << world.get_surface(local_x, local_y) }
                    end
                end

                newVal = vals.inject(0, &:+) / vals.size

                while newVal != thisVal
                    if newVal > thisVal
                        world.set_tile_material(x, y, thisVal+1, thisType)
                        thisVal += 1
                    else
                        world.set_tile_empty(x, y, thisVal)
                        thisVal -= 1
                    end
                end
            end
        end
    end

    def self.generate_riverbeds(world, num_sources=1)
        $logger.info "Generating riverbeds"

        # Generate seed points to build rivers off of
        seed_points  = Array.new(num_sources) { [rand(world.width), rand(world.height)] }
        seed_points.collect! { |point| point << world.get_surface(*point) }
        river_points = []

        # For each seed point, path both upwards and downwards
        seed_points.each do |seed_origin|
            $logger.info "Computing river path for seed point #{seed_origin}"
            river_path = [seed_origin]

            [:ascending, :descending].each do |cardinality|
                position = seed_origin
                while true
                    neighbors = []
                    (position[0]-1..position[0]+1).each do |x|
                        (position[1]-1..position[1]+1).each do |y|
                            next if x==0 && y==0
                            next if world.out_of_bounds?(x, y, 0)
                            neighbors << [x,y,world.get_surface(x,y)]
                        end
                    end
                    # Ensure we don't traverse the same tile twice
                    neighbors.reject! { |n| river_path.include?(n) }
                    break if neighbors.size == 0

                    # Sort the neighboring tiles by height, keeping only the ties for the extreme
                    # Break if we've found the minima or maxima
                    if cardinality == :descending
                        neighbors.sort! { |x,y| x[2] <=> y[2] }
                        minima = neighbors.first
                        neighbors.reject! { |n| n[2] > minima[2] }
                        break if neighbors.first[2] > position[2]
                    else
                        neighbors.sort! { |x,y| y[2] <=> x[2] }
                        maxima = neighbors.first
                        neighbors.reject! { |n| n[2] < maxima[2] }
                        break if neighbors.first[2] < position[2]
                    end
                    # Move a random direction amongst the extrema
                    position = neighbors[rand(neighbors.size)]

                    # Add the current position to the river path
                    river_path << position.dup
                end
            end

            # Add this river path to the river system, weighing each point by how many rivers pass through it
            river_path.each do |point|
                existing_point = river_points.index(point)
                if existing_point.nil?
                    river_points << [point, 1]
                else
                    river_points[existing_point].last += 1
                end
            end
        end

        # Iterate over the river points, queueing up points to be eroded
        erode_points = []
        river_points.each do |point, weight|
            # Erode the landscape at each point the river passes through using a tapered window
            # This has the effect of rolling a (rough) sphere across the landscape, making a dent as it rolls
            # The size of our window is determined by the breadth of the river (how many river seeds pass through this point)
            erode_points.concat(compute_spherical_batch(weight, 0.0, point))
        end

        erode_points.uniq!
        erode_points.reject! { |point| world.out_of_bounds?(*point) }
        erode_points.each { |point| world.set_tile_empty(*point) }
    end

    def self.fill_ocean(world, liquid_type_klass)
        liquid_type = liquid_type_klass.class_attributes[:material_id]

        # Take an average of the landscape height
        average_height = 0
        (0...world.width).each do |x|
            (0...world.height).each do |y|
                average_height += world.get_surface(x,y)
            end
        end
        average_height /= (world.width * world.height)
        $logger.info "Filling oceans to depth #{average_height}"

        # Fill water to this level
        # TODO - Add some variation here to make this builder step more sophisticated and interesting
        (0...world.width).each do |x|
            (0...world.height).each do |y|
                surface_level = world.get_surface(x,y)
                ((surface_level+1)..average_height).each do |z|
                    # TODO - Make set_tile_material smart enough to do its own lookups and set properties
                    world.set_tile_material(x,y,z,liquid_type)
                    world.terrain.set_tile_property(x,y,z,TileProperty.Liquid,true)
                end
            end
        end
    end

    # =================
    # Utility functions
    # =================

    # Lower a single column of tiles at x,y by amount z-levels
    def self.drop_column(world,x,y,amount)
        s_level = world.terrain.get_surface(x, y)
        offset = [s_level, amount].min
        (0..s_level).each do |z_level|
            if (z_level + offset) < world.depth && (z_level + offset) <= s_level
                new_type = world.get_tile_material(x, y, z_level + offset)
                world.set_tile_material(x, y, z_level, new_type)
            else
                world.set_tile_empty(x, y, z_level)
            end
        end
    end

    def self.compute_spherical_batch(dimension, irregularity, offset=[0,0,0])
        radius = dimension.to_i

        results = []
        (-radius..radius).each do |x|
            (-radius..radius).each do |y|
                (-radius..radius).each do |z|
                    if (x**2) + (y**2) + (z**2) <= (radius**2 + (rand * dimension * irregularity))
                        results << [x+offset[0],y+offset[1],z+offset[2]]
                    end
                end
            end
        end
        results
    end

    def self.compute_elliptic_cross_section(dims, offset=[0,0])
        a = (dims[0].to_i ** 2).to_f
        b = (dims[1].to_i ** 2).to_f

        results = []
        (-dims[0]..dims[0]).each do |x|
            (-dims[1]..dims[1]).each do |y|
                # Use the equation for an ellipse to determine if [x,y] is in the cross-section
                if (x**2 / a) + (y**2 / b) <= 1.0
                    results << [x+offset[0],y+offset[1]]
                end
            end
        end
        results
    end

    def self.batch_set_material(world, material, batch)
        batch.each do |dims|
            world.set_tile_material(dims[0], dims[1], dims[2], material)
        end
    end

    def self.batch_set_empty(world, batch)
        batch.each do |dims|
            world.set_tile_empty(dims[0], dims[1], dims[2])
        end
    end

    # This is just a unit test, basically
    def self.test_populate(world)
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
            new_pt  = [rand(world.width), rand(world.height), rand(world.depth)]
            new_val = rand(value_range)
            if not test_points.include? new_pt
                test_points << new_pt
                test_values << new_val
            end
        end

        # Set values
        $logger.info "=Seeding test points"
        test_points.each_with_index { |pt,ind| world.set_tile_material(pt[0], pt[1], pt[2], test_values[ind]) }

        # Verify that values emerge the same as when they went in
        $logger.info "=Verifying test points..."
        test_points.each_with_index do |pt, ind|
            ret_val = world.get_tile_material(pt[0], pt[1], pt[2])
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

    def self.voronois(size, features=nil, n=2, coeff=[-1,1])
        # Derive an appropriate number of features based on the size if necessary
        features ||= (size / 10).to_i
        features = [features,n].max

        $logger.info "Computing a voronois layer with #{features} features using a length #{n} polynomial"

        # Initialize the grid
        @array = Array.new(size) { Array.new(size,0) }

        # Check the arguments for sanity
        if coeff.size != n
            $logger.info "Bad arguments passed to voronois generation"
            return @array
        end

        # Generate feature points
        feature_points = []
        until feature_points.size == features do
            feature_points << [rand(size), rand(size)]
            feature_points.uniq!
        end

        # Iterate across the diagram, setting the magnitude of each point
        #  according to its distance from the features
        size.times do |x|
            size.times do |y|
                # Compute the distance to each feature
                # FUN!
                xP = x + Math.sin(y)
                yP = y - Math.sin(x)
                feat_dist = feature_points.collect { |feature| HeightMap.distance(feature, [xP, yP]) }
                #feat_dist = feature_points.collect { |feature| HeightMap.distance(feature, [x,y]) }
                # Sort the features by distance
                feat_dist.sort!

                # Compute the value at this location using the closest n features
                h_value = 0
                (0...n).each do |index|
                    h_value += (feat_dist[index] * coeff[index])
                end
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
