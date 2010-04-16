require 'Terrain'

class TerrainBuilder
    # Generates a new heightmap and layers it *on top* of any existing terrain
    def self.add_layer(terrain, type, offset=0.0, scale=1.0, entropy=10.0, granularity=0.4)
        $logger.info("Adding new layer : type #{type} scale #{scale} entropy #{entropy} granularity #{granularity}")
        offset = [offset, 1.0-scale].min

        rough_layer = HeightMap.generate(terrain.width, entropy, granularity)
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
        
        rough_layer = HeightMap.generate(terrain.width, entropy, granularity)
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

    def self.shear(terrain, size=5, fault_strays=false, variation=1, cap)
        $logger.info "Shearing terrain: magnitude #{size} fault_strays #{fault_strays} variation #{variation} cap #{cap}"
        # Generate two points at one edge of the terrain space and move to the other, 
        #  randomly moving up and down along the way
        # Randomly generate whether the fault begins on the x or y axis
        start = []
        dir   = []

        r_dir = Proc.new {
            n_val = (rand(1+(variation*2))-variation).to_i
            @n_dir = (@n_dir ? @n_dir + n_val : n_val)
            (@n_dir = [[@n_dir, cap].min, -cap].max) if cap
            (fault_strays ? @n_dir : n_val)
        }
        p_one = Proc.new {  1 }
        n_one = Proc.new { -1 }

        polarity = rand(2).to_i
        axis     = rand(2).to_i

        if axis == 0
            dir << r_dir
            start << ((terrain.width  - 1) * rand(100) / 100.0).to_i
            (rand(2).to_i == 0) ? (start << terrain.height - 1; dir << n_one) : (start << 0; dir << p_one)

            upper_bound = terrain.width
            iterations  = terrain.height
        else
            (rand(2).to_i == 0) ? (start << terrain.width - 1; dir << n_one) : (start << 0; dir << p_one)
            start << ((terrain.height - 1) * rand(100) / 100.0).to_i
            dir << r_dir

            upper_bound = terrain.height
            iterations  = terrain.width
        end

        c_position = start

        iterations.times do
            c_position[0] = [[c_position[0], 0].max, terrain.width - 1 ].min
            c_position[1] = [[c_position[1], 0].max, terrain.height - 1].min

            swath = ((polarity == 0) ? (0..c_position[axis]) : (c_position[axis]...upper_bound))
            swath.each do |coord|
                c = ((axis == 0) ? [coord, c_position[1]] : [c_position[0], coord])
                drop_column(terrain, c[0], c[1], size)
            end

            c_position[0] += dir[0].call
            c_position[1] += dir[1].call
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
                    if !terrain.out_of_bounds?(neighbor[0], neighbor[1])
                        neighbor[2].times { vals << terrain.get_surface(neighbor[0], neighbor[1]) }
                    end
                end

                newVal = vals.inject { |sum,val| sum ? sum+val : val } / vals.size

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
                break if terrain.out_of_bounds?(position[0], position[1])

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
                break if terrain.out_of_bounds?(position[0], position[1])

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

        # Iterate over the river points, eroding the landscape and adding liquid
        river_points.each_with_index do |point,i|
            # Find the direction the river is travelling by taking a difference
            #  between the current point in the river and one adjacent to it
            direction=[0,0]
            if river_points.size > (i+1)
                next_point = river_points[i+1]
                (0..1).each { |j| direction[j] = next_point[j] - point[j] }
            elsif 0 <= (i-1)
                prev_point = river_points[i-1]
                (0..1).each { |j| direction[j] = point[j] - prev_point[j] }
            else
                $logger.info "Rivers too short!  Aborting riverbed generation."
                return
            end
            
            # Create a slice of terrain to erode based on the current point and
            #  the width of the river here (as defined by the number of river seeds
            #  that pass through this point
            # Generate a slice tangential to the river direction to give the river breadth
            breadth = point[2]
            (-breadth..breadth).each do |b|
                erode_x      = point[1] * (-b)
                erode_y      = point[0] * ( b)
                erode_amount = breadth.abs - b

                drop_column(terrain, erode_x, erode_y, erode_amount)
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

    def self.generate(size, localEntropy, granularity, level=2)
        $logger.info "Generating height map: size #{size} localEntropy #{localEntropy} granularity #{granularity} level #{level}"
        @array = Array.new(size) { Array.new(size,0) }
        
        if level + 1 < size
            generate(size, localEntropy / granularity, granularity, level*2)
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
