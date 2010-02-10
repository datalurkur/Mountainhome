#!../ruby/bin/ruby

require 'World.rb'
require 'MountainHomeDSL.rb'

$bedrock = 0
$hardrock = 1
$softrock = 2
$sediment = 3
$air = 4
$water = 5
$wet = 6

def LOG(message)
	puts message
end

def linInterp(array, value)
	array.each_with_index do |a, i|
		if value < a
			return i
		end
	end
	return array.length
end

class WorldFactory
	def initialize(params={})
		@entropy     = params[:entropy]     || 10.0
		@granularity = params[:granularity] || 0.6
	end

	def generateWorld(breadth, depth)
		# Compute dimensions and instantiate the new world object
		dims = [2 ** breadth + 1, 2 ** breadth + 1, 2 ** depth +1]
		world = World.new(:width => dims[0], :height => dims[1], :depth => dims[2])

		# Generate a random seed based on the current unix epoch time
		srand(Time.now.to_i)

		# Set up layer types
		types = ["Bedrock", "Hardrock", "Softrock", "Sediment"]
		numLayers = types.length

		# Instantiate our arrays
		layers = []
		numLayers.times {|d| layers << Array.new(dims[0]) { Array.new(dims[1],0) } }

		# Generate the terrain layers
		layers.each {|layer| generateLayer(2, dims[0], @entropy, layer)}
		
#		sampledArray = sampleZLayers(2**depth_pwr, bedrock, hardrock, softrock, sediment)

		compositeHeight = Array.new(dims[0]) { Array.new(dims[1]) }
		layerCutoffs = Array.new(dims[0]) { Array.new(dims[1]) { Array.new(numLayers) } }

		# Computer layer minima and scale the layers into positive space
		lMins = layers.collect { |l| l.collect(&:min).min }
		(0...dims[0]).each do |x|
			(0...dims[1]).each do |y|
				layers.each_with_index do |layer,i|
					compositeHeight[x][y]=0.0 if not compositeHeight[x][y]
					compositeHeight[x][y] += ((lMins[i] < 0) ? layer[x][y]-lMins[i] : layer[x][y])
					layerCutoffs[x][y][i]=compositeHeight[x][y]
				end
			end	
		end

		# Determine the composite layer's minima and maxima
		cMin = compositeHeight.collect(&:min).min
		cMax = compositeHeight.collect(&:max).max

		# Based on these numbers, determine the scaling factor and offset
		scale = dims[2] / cMax

		(0...dims[0]).each do |x|
			(0...dims[1]).each do |y|
				(layerCutoffs[x][y]).collect! { |c| c*scale }
				(0...dims[2]).each do |z|
					index = linInterp(layerCutoffs[x][y], z)
					tile = eval(types[index] || "Empty").new
					world.setTile(x,y,z,tile)
				end
			end
		end

		world
	end

	def generateLayer(level, size, localEntropy, array)
		if level + 1 < size
			generateLayer(level*2, size, localEntropy / @granularity, array)
		else
			# Instantiate an array of random start values
			iVals=Array.new(4)
			iRange = localEntropy / @granularity
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
	def calcBox(lowerX, lowerY, upperX, upperY, localEntropy, array)
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
	def calcDiamond(lowerX, lowerY, upperX, upperY, size, localEntropy, array)
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

worldgenerator = WorldFactory.new
world = worldgenerator.generateWorld(3, 2)
