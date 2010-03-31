require 'Terrain'

class TerrainBuilder
    def self.add_layer(terrain, type, scale=1.0, entropy=10.0, granularity=0.4)
        rough_layer = HeightMap.generate(terrain.width, entropy, granularity)
        layer = HeightMap.scale(1, scale*(terrain.depth-1), rough_layer)
        
        layer.each_with_index do |row, x|
            row.each_with_index do |col, y|
                offset = 1 + terrain.get_surface(x, y)
                range = (0..col.floor)
                range.each { |z| terrain.set_tile(x, y, (z + offset), type) if ((z+offset) < terrain.depth)}
            end
        end
        
        #puts "Here comes the layer..."
        #layer.each do |row|
        #    nRow = row.collect { |c| c.to_i.to_s }
        #    puts nRow.pack("A3"*nRow.size)
        #end

        terrain
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

        array
    end

    def self.generate(size, localEntropy, granularity, level=2)
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
