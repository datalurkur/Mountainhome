#!ruby/bin/ruby
require 'rubygems'
require 'RMagick'
include Magick

$bedrock = 0
$hardrock = 1
$softrock = 2
$sediment = 3
$air = 4
$water = 5
$wet = 6

def mda(width,height,*default)
	Array.new(width) { Array.new(height, default[0]) }
end

def linInterp(array, value)
	array.each_with_index do |a, i|
		if value < a
			return i
		end
	end
	return array.length
end

$log_level = 2

def LOG(message, level)
	puts message if (level >= $log_level)
end

# The first step in midpoint heightmapping
def calcBox(lowerX, lowerY, upperX, upperY, entropy, array)
	# Calculate our halfway points
	offset = (upperX - lowerX) / 2.0
	middleX = lowerX + offset
	middleY = lowerY + offset

	# Compute the size of intersection of the diagonals
	# as the average of the four corners plus a random amount
	middle = array[lowerX][lowerY] + array[lowerX][upperY] + array[upperX][lowerY] + array[upperX][upperY]
	middle /= 4.0
	middle += (rand(entropy) - (entropy/2.0))

	array[middleX][middleY] = middle
end

# The second step in midpoint heightmapping
def calcDiamond(lowerX, lowerY, upperX, upperY, entropy, size, array)
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
	if preX < 0
		midLeft *= 4.0 / 3.0
	else
		midLeft += array[preX][middleY]
	end
	midLeft /= 4.0
	midLeft += (rand(entropy) - (entropy/2.0))
	
	midRight += array[upperX][lowerY] + array[upperX][upperY]
	if postX >= size
		midRight *= 4.0 / 3.0
	else
		midRight += array[postX][middleY]
	end
	midRight /= 4.0
	midRight += (rand(entropy) - (entropy/2.0))
	
	midTop += array[lowerX][upperY] + array[upperX][upperY]
	if postY >= size
		midTop *= 4.0 / 3.0
	else
		midTop += array[middleX][postY]
	end
	midTop /= 4.0
	midTop += (rand(entropy) - (entropy/2.0))
	
	midBottom += array[lowerX][lowerY] + array[upperX][lowerY]
	if preY < 0
		midBottom *= 4.0 / 3.0
	else
		midBottom += array[middleX][preY]
	end
	midBottom /= 4.0
	midBottom += (rand(entropy) - (entropy/2.0))
	
	array[lowerX][middleY] = midLeft
	array[upperX][middleY] = midRight
	array[middleX][upperY] = midTop
	array[middleX][lowerY] = midBottom
end

def genTerrain(level, size, entropy, granularity, array)
	if level + 1 < size
		genTerrain(level*2, size, entropy/granularity, granularity, array)
	else

		# Instantiate an array of random start values
		iVals=Array.new(4)
		iRange = entropy/granularity
		iVals.collect! { |x| rand(iRange)-(iRange/2.0) }

		# Seed the terrain
		LOG("Seeding initial indices with range #{iRange} and values #{iVals.inspect}",2)
		array[0][0]           = iVals[0]
		array[0][size-1]      = iVals[1]
		array[size-1][0]      = iVals[2]
		array[size-1][size-1] = iVals[3]
	end

	# The highest level will be computed first, with 1 segment
	segments = (size - 1) / level
	LOG("Computing #{segments} segments for level #{level} with params #{entropy}, #{granularity}",2)

	# First, compute the midpoints of all the sections
	segments.times do |x|
		segments.times do |y|
			# Divide the grid into segments ** 2 pieces
			calcBox(x*level, y*level, (x+1)*level, (y+1)*level, entropy, array)
		end
	end

	# Then, use the midpoint calculations to compute the halfway heights for edges
	segments.times do |x|
		segments.times do |y|
			calcDiamond(x*level, y*level, (x+1)*level, (y+1)*level, entropy, size, array)
		end
	end
end

def genImage(rscale, array, ocean)
	size = array.length
	LOG("Generating an image with resolution #{size}x#{size}",2)
	LOG("Computing minima and maxima",1)
	max = array.collect(&:max).max
	min = array.collect(&:min).min
	
	image = Image.new(size, size)
	offset = -min
	scale = 255.0 / (max - min)
	
	pixels = Array.new(size**2) 
	(0...size).each do |x|
		(0...size).each do |y|
			val = (array[x][y] + offset) * scale
			
			pixel = Pixel.new
			if not ocean or not ocean[x][y]
				pixel.red = val.to_i
				pixel.green = val.to_i
			end
			pixel.blue = val.to_i
			pixels[x*size+y]=pixel
		end
	end
	
	image.store_pixels(0,0,size,size,pixels)
	if rscale != 1.0
		image.resize!(size*rscale, size*rscale, filter=BoxFilter, support=1.0)
	end
	return image
end

def sampleZLayers(depth, *layers)
	if layers.empty?
		return nil
	end

	size = layers[0].length
	heightMap = mda(size, size)
	columns = mda(size, size)

	if layers.inject { |mismatch, l| (l.length != size) ? true : false }
		LOG("**ERROR: Layer dimensional mismatch!",2)
		return nil
	end

	LOG("Generating composite heightmap...",2)
	lMins = layers.collect { |l| l.collect(&:min).min }
	(0...size).each do |x|
		(0...size).each do |y|
			columns[x][y]=Array.new(layers.length)
			layers.each_with_index do |layer,i|
				# Make this easy on ourselves later by shifting the layers into positive space now
				# Also keep track of the heights of the different layers in an array
				if not heightMap[x][y]
					heightMap[x][y]=0.0
				end
				heightMap[x][y] += ((lMins[i] < 0) ? layer[x][y]-lMins[i] : layer[x][y])
				columns[x][y][i]=heightMap[x][y]
			end
		end	
	end
	LOG("Done!",2)

	# Determine the composite layer's minima and maxima
	cMin = heightMap.collect(&:min).min
	cMax = heightMap.collect(&:max).max
	LOG("Composite layer has minima and maxima #{[cMin,cMax].inspect}",1)
	if cMin < 0
		LOG("**ERROR: Minima less than zero",1)
	end

	# Based on these numbers, determine the scaling factor and offset
	scale = depth / cMax
	LOG("Scaling heightmap by a factor of #{scale}",1)

	# Instantiate our 3-dimensional array 
	zArray = mda(size, size) 

	LOG("Populating 3-dimensional array...",2)
	(0...size).each do |x|
		(0...size).each do |y|
			(columns[x][y]).collect! { |c| c*scale }
			zArray[x][y] = Array.new(depth)
			(0...depth).each do |z|
				zArray[x][y][z] = Tile.new(linInterp(columns[x][y], z), 0)
			end
		end
	end
	LOG("Done!",2)

	return zArray
end

# Turn a 3-dimensional array into a series of images and generate a composite
def generateSliceMap(array, scale, filename)
	LOG("="*75,2)
	LOG("Generating a slicemap of a 3-dimensional array...",2)
	# First, determine the dimensions of the composite image
	width = array.length
	if width <= 0
		return nil
	end
	height = array[0].length
	if height <= 0
		return nil
	end
	depth = array[0][0].length
	if depth <= 0
		return nil
	end
	LOG("#{depth} images of size #{[width,height].inspect} will be composited", 1)

	dim = depth**0.5
	rows=cols=0
	if dim.to_i != dim
		cols = depth
		rows = 1
		until (rows >= cols) do
			rows*=2
			cols/=2
		end
	else
		rows = cols = dim.to_i
	end

	LOG("Slicemap will be generated with #{rows} rows and #{cols} columns", 1)
	iWidth = rows*(width+1)
	iHeight = cols*(height+1)
	LOG("Temporarily storing pixels in an array of length #{iWidth*iHeight}", 1)
	pixelArray = Array.new(iWidth*iHeight)

	(0...depth).each do |z|
		xOffset = (z / rows) * (width+1)
		yOffset = (z % rows) * (height+1)
		LOG("~ Compositing slice #{z} with offsets #{[xOffset,yOffset].inspect}", 1)
		(0..width).each do |x|
			(0..height).each do |y|
				pixelX = xOffset + x
				pixelY = yOffset + y
				if (x==width or y==height)
					pixelArray[pixelX*iWidth+pixelY]=Pixel.new(0,0,0,1)
				else
					pixelArray[pixelX*iWidth+pixelY]=getColorPixel(array[x][y][z].type)
				end
			end
		end
	end

	sliceMap = Image.new(iWidth, iHeight)
	LOG("Producing slice map image with dimensions #{[iWidth,iHeight].inspect}",1)
	sliceMap.store_pixels(0,0,iWidth,iHeight,pixelArray)
	if scale != 1.0
		sliceMap.resize!(iWidth*scale, iHeight*scale, filter=BoxFilter, support=1.0)
	end
	sliceMap.write(filename)
	LOG("Slice map image saved.",1)
	LOG("#{sliceMap.inspect}",1)
end

def getColorPixel(index)
	if index == -1
		LOG("**ERROR: Unfilled index found!",2)
		return nil
	elsif index == $bedrock
		return Pixel.new(100,75,40,255)
	elsif index == $hardrock
		return Pixel.new(150,115,60,255)
	elsif index == $softrock
		return Pixel.new(200,150,80,255)
	elsif index == $sediment
		return Pixel.new(255,185,100,255)
	elsif index == $air
		return Pixel.new(150,150,255,255)
	elsif index == $water
		return Pixel.new(0,0,255,255)
	elsif index == $wet
		return Pixel.new(100,255,100,255)
	else
		LOG("**ERROR: Unknown index - #{index} - found!",2)
		return nil
	end
end

class Tile
	attr_accessor :type, :val
	def initialize(type, val)
		@type=type
		@val=val
	end
end

class Coord
	attr_accessor :x, :y, :z
	def initialize(x,y,z)
		@x=x
		@y=y
		@z=z
	end
end

def simRainfall(array, iterations)
	raindrops=iterations-50
	intensity=1000
	maxValue=1000

	LOG("Simulating rainfall...",2)

	maxX = array.length
	maxY = array[0].length
	maxZ = array[0][0].length

	drops = Array.new
	(1..iterations).each do |iter|
		LOG("~ Iteration #{iter}", 1)
		
		if raindrops > 0
			# Generate raindrop
			#rX = (maxX/2.0).to_i
			#rY = (maxY/2.0).to_i
			rX = rand(maxX)
			rY = rand(maxY)
			rZ = maxZ-1
			rVal = 10
			LOG("+ Generating a raindrop at #{[rX,rY,rZ].inspect}", 1)

			if array[rX][rY][rZ].type != $air
				LOG("** ERROR - tile is occupied",2)
			else
				array[rX][rY][rZ].type = $water
				array[rX][rY][rZ].val  = intensity
				drops.push(Coord.new(rX,rY,rZ))
			end
			raindrops -= 1
		end

		drops.each do |drop|
			x = drop.x
			y = drop.y
			z = drop.z
			value = array[x][y][z].val
			
			LOG("+ Processing drop at #{[x,y,z].inspect}", 1)
			# Is drop resting or falling?
			if z > 0 and array[x][y][z-1].type == $air
				# Drop is falling
				array[x][y][z  ].type = $air
				array[x][y][z  ].val  = 0
				array[x][y][z-1].type = $water
				array[x][y][z-1].val  = value
				drop.z = z-1
				LOG("-- Drop falls", 1)
			elsif z > 0
				# Drop has hit something
				# Check to see if it hit more water
				if array[x][y][z-1].type == $water
					# Is there room in the tile below for this drop?
					if array[x][y][z-1].val + value <= maxValue
						LOG("--- Drop merges with one below", 1)
						# There is, combine them and skip to the next drop
						array[x][y][z-1].val += value
						array[x][y][z].type = $air
						array[x][y][z].val = 0
						drops.delete(drop)
						next
					else
						# There isn't, combine as much as possible and continue processing what's left of this drop
						space = maxValue - array[x][y][z-1].val
						array[x][y][z-1].val = maxValue
						array[x][y][z].val = value - space
					end
				end
				
				# Spread the drop out to unfilled adjacent tiles
				# Get adjacent tiles one z-level down
				adjTiles = []
				adjTiles.push( ((x > 0)      ? Coord.new(x-1, y,   z-1) : nil) )
				adjTiles.push( ((y > 0)      ? Coord.new(x,   y-1, z-1) : nil) )
				adjTiles.push( ((x < maxX-1) ? Coord.new(x+1, y,   z-1) : nil) )
				adjTiles.push( ((y < maxY-1) ? Coord.new(x,   y+1, z-1) : nil) )

				# Determine which adjacent tiles are eligible for droplet drainage
				eligible = adjTiles.collect { |tile| (tile and array[tile.x][tile.y][tile.z].type == $air) ? tile : nil }
				opens = eligible.compact.size

				if opens > 0
					# Determine new value
					LOG("-- #{opens} eligible tiles for outflow, flowing out.", 1)
					newValue = (value/opens).to_i
					LOG("-- New value: #{newValue}", 1)
					if newValue <= 0
						# Not enough to go around, delete this droplet
						LOG("Not enough volume to drain, mistifying...", 1)
						array[drop.x][drop.y][drop.z].type = $air
						array[drop.x][drop.y][drop.z].val  = 0
						drops.delete(drop)
						next
					end

					# Toast the old droplet
					LOG("-- Removing droplet at #{[x,y,z].inspect}", 1)
					array[x][y][z].type = $air
					array[x][y][z].val  = 0
					drops.delete(drop)
					array[x][y][z-1].type = $wet

					# Create new droplets
					eligible.compact.each do |tile|
						LOG("--- Creating new droplet at #{[tile.x,tile.y,tile.z].inspect}", 1)
						array[tile.x][tile.y][tile.z].type = $water
						array[tile.x][tile.y][tile.z].val  = newValue
						drops.push(Coord.new(tile.x, tile.y, tile.z))
					end
				else
					LOG("-- No eligible tiles for outflow, spreading.", 1)
					adjTiles = []
					adjTiles.push( (drop.x > 0      ? Coord.new(drop.x-1, drop.y,   drop.z) : nil) )
					adjTiles.push( (drop.y > 0      ? Coord.new(drop.x,   drop.y-1, drop.z) : nil) )
					adjTiles.push( (drop.x < maxX-1 ? Coord.new(drop.x+1, drop.y,   drop.z) : nil) )
					adjTiles.push( (drop.y < maxY-1 ? Coord.new(drop.x,   drop.y+1, drop.z) : nil) )
					eligible = adjTiles.collect {|tile| (tile and (array[tile.x][tile.y][tile.z].type == $air or array[tile.x][tile.y][tile.z].type == $water)) ? tile : nil }
					opens=eligible.compact.size+1
					if opens > 0
						LOG("-- #{opens} eligible tiles for spread, spreading.", 1)
						newValue = value
						eligible.compact.each { |tile| newValue+=array[tile.x][tile.y][tile.z].val }
						newValue /= opens
						LOG("-- New value: #{newValue}", 1)
						
						if newValue <= 0
							LOG("Not enough volume to spread, vaporizing...", 1)
							array[drop.x][drop.y][drop.z].type = $air
							array[drop.x][drop.y][drop.z].val  = 0
							drops.delete(drop)
							next
						end
						
						# Set this tile's new value
						array[x][y][z].val = newValue
						
						# Set the new types and values, and add droplets where necessary
						eligible.compact.each do |tile|
							if array[tile.x][tile.y][tile.z].type != $water
								LOG("--- Adding new droplet at #{[tile.x, tile.y, tile.z].inspect}", 1)
								array[tile.x][tile.y][tile.z].type = $water
								drops.push(Coord.new(tile.x, tile.y, tile.z))
							else
								LOG("--- Modifying existing droplet at #{[tile.x,tile.y,tile.z].inspect}", 1)
							end
							array[tile.x][tile.y][tile.z].val = newValue
						end
					else
						LOG("-- No tiles to spread to, drop idles.", 1)
						drops.delete(drop)
					end
				end
			else
				# Drop has hit the bottom, break out
				LOG("** ERROR - Droplet has escaped!",2)
			end	
		end
	end
end

# Shift in command line params / set defaults
entropy     = (ARGV.shift || 10.0).to_f
granularity = (ARGV.shift || 0.6  ).to_f
sz_pwr      = (ARGV.shift || 6    ).to_i
depth_pwr   = (ARGV.shift || 4    ).to_i
rsz_scale   = (ARGV.shift || 4.0  ).to_f

LOG("Commencing worldgen with parameters #{entropy}, #{granularity} and scale #{sz_pwr}",2)
LOG("="*75,2)
if granularity > 0.8
	LOG("Warning, abnormally high granularity parameter, expected the unexpected.",2)
end

# Generate a random seed based on the current unix epoch time
srand(Time.now.to_i)

# Compute our maximum size based on the input params
max_size=2**sz_pwr + 1

# Instantiate our arrays
bedrock  = mda(max_size, max_size)
hardrock = mda(max_size, max_size)
softrock = mda(max_size, max_size)
sediment = mda(max_size, max_size)
world    = mda(max_size, max_size)

LOG("Generating bedrock layer",2)
genTerrain(2, max_size, entropy, granularity, bedrock)
bImage = genImage(rsz_scale, bedrock, nil)

LOG("="*75,2)
LOG("Generating hard rock layer",2)
genTerrain(2, max_size, entropy, granularity, hardrock)
hImage = genImage(rsz_scale, hardrock, nil)

LOG("="*75,2)
LOG("Generating soft rock layer",2)
genTerrain(2, max_size, entropy, granularity, softrock)
rImage = genImage(rsz_scale, softrock, nil)

LOG("="*75,2)
LOG("Generating sedimentary layer",2)
genTerrain(2, max_size, entropy, granularity, sediment)
sImage = genImage(rsz_scale, sediment, nil)

LOG("="*75,2)
LOG("Combining layers...",2)
sampledArray = sampleZLayers(2**depth_pwr, bedrock, hardrock, softrock, sediment)
rainArray = Marshal.load(Marshal.dump(sampledArray))
simRainfall(rainArray, 100)

makeImages = true
if makeImages
	LOG("="*75,2)
	LOG("Generating composite heightmap...",2)
	iSize=max_size*rsz_scale
	fullImage = Image.new(iSize*2+2, iSize*2+2)
	fullImage.store_pixels(0,       0,           iSize, iSize, bImage.get_pixels(0,0,iSize,iSize))
	fullImage.store_pixels(iSize+1, 0,           iSize, iSize, hImage.get_pixels(0,0,iSize,iSize))
	fullImage.store_pixels(0,       iSize+1,     iSize, iSize, rImage.get_pixels(0,0,iSize,iSize))
	fullImage.store_pixels(iSize+1, iSize+1,     iSize, iSize, sImage.get_pixels(0,0,iSize,iSize))
	fullImage.write("heightmaps.jpg")
	LOG("Heightmap composite saved.",2)
	LOG("#{fullImage.inspect}",2)
	generateSliceMap(sampledArray, rsz_scale, "slicemap.jpg")
	generateSliceMap(rainArray, rsz_scale, "rainmap.jpg")
end

exit
