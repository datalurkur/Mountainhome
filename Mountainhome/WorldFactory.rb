require 'World'

class WorldFactory
  def self.generateWorld(core, breadth, depth=[0, breadth - 2].max, entropy = 10.0, granularity = 0.4, gen_seed = Time.now.to_i)
    # Compute dimensions and instantiate the new world object
    dims = [2 ** breadth + 1, 2 ** breadth + 1, 2 ** depth +1]

    # Generate a random seed based on the current unix epoch time
    srand gen_seed
    $logger.info "Generating world."
    $logger.info "    Dimensions: #{dims.join(" x ")}"
    $logger.info "    Seed: #{gen_seed}"

    world = World.new(dims[0], dims[1], dims[2], core)

    # Set up layer types
    types = [Bedrock, Hardrock, Softrock, Sediment]

    # Instantiate our arrays
    layers = Array.new(types.length) { Array.new(dims[0]) { Array.new(dims[1],0) } }

    $logger.info "Generate the terrain layers"
    layers.each {|layer| generateLayer(2, dims[0], entropy, layer, granularity)}

    compositeHeight = Array.new(dims[0]) { Array.new(dims[1]) }
    layerCutoffs = Array.new(dims[0]) { Array.new(dims[1]) { Array.new(types.length) } }

    $logger.info "Computer layer minima and scale the layers into positive space"
    lMins = layers.collect { |l| l.collect(&:min).min }
    (0...dims[0]).each do |x|
      (0...dims[1]).each do |y|
        layers.each_with_index do |layer,i|
          compositeHeight[x][y] = 0.0 if not compositeHeight[x][y]
          compositeHeight[x][y] += ((lMins[i] < 0) ? layer[x][y]-lMins[i] : layer[x][y])
          layerCutoffs[x][y][i] = compositeHeight[x][y]
        end
      end
    end

    # Determine the composite layer's minima and maxima
    cMin = compositeHeight.collect(&:min).min
    cMax = compositeHeight.collect(&:max).max

    # Based on these numbers, determine the scaling factor and offset.
    scale = (dims[2] - 1) / cMax

    $logger.info "Scale the layers into the integer space of the depth"
    (0...dims[0]).each do |x|
      (0...dims[1]).each do |y|
        (layerCutoffs[x][y]).collect! { |c| c * scale }
    #    (0...dims[2]).each do |z|
    #      index = linInterp(layerCutoffs[x][y], z)
    #      tile = types[index] || Empty.new
    #      world.setTile(x,y,z,tile)
    #    end
      end
    end

    $logger.info "Loop through the tiles, registering them with the world."
    layerCutoffs.each_with_index do |row,x|
      row.each_with_index do |col,y|
        ranges = []
        last_index = 0
        col.each_with_index do |level, z|
          ranges << [types[z], (last_index..level.floor)]
          last_index = level.floor
        end

        ranges.each do |type, range|
            range.each { |z| world.update_tile(type, x, y, z) }
        end
      end
    end

    world
  end

  def self.generateLayer(level, size, localEntropy, array, granularity)
    if level + 1 < size
      generateLayer(level*2, size, localEntropy / granularity, array, granularity)
    else
      # Instantiate an array of random start values
      iVals=Array.new(4)
      iRange = localEntropy / granularity
      iVals.collect! { |x| rand(iRange)-(iRange/2.0) }

      # Seed the terrain
      array[0][0]           = iVals[0]
      array[0][size-1]      = iVals[1]
      array[size-1][0]      = iVals[2]
      array[size-1][size-1] = iVals[3]
    end

    # The highest level will be computed first, with 1 segment
    segments = (size - 1) / level

    # First, compute the midpoints of all the sections
    segments.times do |x|
      segments.times do |y|
        # Divide the grid into segments ** 2 pieces
        calcBox(x*level, y*level, (x+1)*level, (y+1)*level, localEntropy, array)
      end
    end

    # Then, use the midpoint calculations to compute the halfway heights for edges
    segments.times do |x|
      segments.times do |y|
        calcDiamond(x*level, y*level, (x+1)*level, (y+1)*level, size, localEntropy, array)
      end
    end
  end

  # The first step in midpoint heightmapping
  def self.calcBox(lowerX, lowerY, upperX, upperY, localEntropy, array)
    # Calculate our halfway points
    offset = (upperX - lowerX) / 2.0
    middleX = lowerX + offset
    middleY = lowerY + offset

    # Compute the size of intersection of the diagonals
    # as the average of the four corners plus a random amount
    middle = array[lowerX][lowerY] + array[lowerX][upperY] + array[upperX][lowerY] + array[upperX][upperY]
    middle /= 4.0
    middle += (rand(localEntropy) - (localEntropy/2.0))

    array[middleX][middleY] = middle
  end

  # The second step in midpoint heightmapping
  def self.calcDiamond(lowerX, lowerY, upperX, upperY, size, localEntropy, array)
    # Compute indices
    offset = (upperX - lowerX) / 2.0
    middleX = lowerX + offset
    middleY = lowerY + offset
    preX = lowerX - offset
    postX = upperX + offset
    preY = lowerY - offset
    postY = upperY + offset
    
    midLeft = midRight = midTop = midBottom = array[middleX][middleY]

    midLeft += array[lowerX][lowerY] + array[lowerX][upperY]
    (preX < 0) ? midLeft *= 4.0 / 3.0 : midLeft += array[preX][middleY]
    midLeft /= 4.0
    midLeft += (rand(localEntropy) - (localEntropy/2.0))
    
    midRight += array[upperX][lowerY] + array[upperX][upperY]
    (postX >= size) ? midRight *= 4.0 / 3.0 : midRight += array[postX][middleY]
    midRight /= 4.0
    midRight += (rand(localEntropy) - (localEntropy/2.0))
    
    midTop += array[lowerX][upperY] + array[upperX][upperY]
    (postY >= size) ? midTop *= 4.0 / 3.0 : midTop += array[middleX][postY]
    midTop /= 4.0
    midTop += (rand(localEntropy) - (localEntropy/2.0))
    
    midBottom += array[lowerX][lowerY] + array[upperX][lowerY]
    (preY < 0) ? midBottom *= 4.0 / 3.0 : midBottom += array[middleX][preY]
    midBottom /= 4.0
    midBottom += (rand(localEntropy) - (localEntropy/2.0))
    
    array[lowerX][middleY] = midLeft
    array[upperX][middleY] = midRight
    array[middleX][upperY] = midTop
    array[middleX][lowerY] = midBottom
  end
end
