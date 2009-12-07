#!/usr/bin/ruby
require 'rubygems'
require 'RMagick'
include Magick

def mda(width,height,*default)
	Array.new(width) { Array.new(height, default[0]) }
end

def linInterp(array, value)
#	puts "Linearly interpolating to find index for value #{value.inspect} in\n#{array.inspect}"
	array.each_with_index do |a, i|
#		puts "~ Checking against index #{i}"
		if value < a
#			puts "~ #{value} is below index #{i}'s value #{a}, returning #{i}"
			return i
		end
	end
#	puts "~ #{value} is above the highest layer, returning #{array.length}"
	return array.length
end

$log_enabled = false

def LOG(message)
	if $log_enabled
		puts message
	end
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
		puts "Seeding initial indices with range #{iRange} and values #{iVals.inspect}"
		array[0][0]           = iVals[0]
		array[0][size-1]      = iVals[1]
		array[size-1][0]      = iVals[2]
		array[size-1][size-1] = iVals[3]
	end

	# The highest level will be computed first, with 1 segment
	segments = (size - 1) / level
	puts "Computing #{segments} segments for level #{level} with params #{entropy}, #{granularity}"

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

def genOceans(size, percent, tolerance, array, ocean)
	upper = array.collect(&:max).max
	lower = array.collect(&:min).min
	sealevel = 0.0
	pass = 0
	cPct = 0.0

	puts "Attempting to generate a #{(percent*100).to_i}% aquatic world with #{(tolerance*100).to_i}% tolerance"

	while (cPct - percent).abs > tolerance or pass < 1
		pass+=1
		count=0.0

		puts "=PASS #{pass}="
		puts "Generating oceans with sealevel #{sealevel} and bounds [ #{lower} , #{upper} ]"
		
		(0...size).each do |x|
			(0...size).each do |y|
				val = array[x][y]
				if val < sealevel
					ocean[x][y]=true
					count+=1.0
				else
					ocean[x][y]=false
				end
			end
		end
		cPct = count / (size**2)
		puts "Produced a #{(cPct*100).to_i}% aquatic world"

		if cPct > percent
			upper = sealevel
			sealevel = lower + ((sealevel - lower) / 2.0)
		elsif cPct < percent
			lower = sealevel
			sealevel = sealevel + ((upper - sealevel) / 2.0) 
		end
	end
	
end

def genImage(rscale, array, ocean)
	size = array.length
	puts "Generating an image with resolution #{size}x#{size}"
	puts "Computing minima and maxima"
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

def sampleZLayers(*layers)
	if layers.empty?
		return nil
	end

	size = layers[0].length
	heightMap = mda(size, size)
	columns = mda(size, size)

	if layers.inject { |mismatch, l| (l.length != size) ? true : false }
		puts "**ERROR: Layer dimensional mismatch!"
		return nil
	end

	puts "Generating composite heightmap..."
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
	puts "...Done!"

	# Determine the composite layer's minima and maxima
	cMin = heightMap.collect(&:min).min
	cMax = heightMap.collect(&:max).max
	puts "Composite layer has minima and maxima #{[cMin,cMax].inspect}"
	if cMin < 0
		puts "**ERROR: Minima less than zero"
	end

	# Based on these numbers, determine the scaling factor and offset
	scale = size / cMax
	puts "Scale: #{scale}"

	# Instantiate our 3-dimensional array 
	zArray = Array.new(size, mda(size,size,Tile.new(-1, 0)))

	puts "Filling 3-dimensional array"
	(0...size).each do |x|
		(0...size).each do |y|
			(columns[x][y]).collect! { |c| c*scale }
			(0...size).each do |z|
				zArray[x][y][z] = linInterp(columns[x][y], z)
			end
		end
	end
end

class Tile
	attr_accessor :type, :val
	def initialize(type, val)
		@type=type
		@val=val
	end
end

# Shift in command line params / set defaults
entropy     = (ARGV.shift || 10.0).to_f
granularity = (ARGV.shift || 0.6  ).to_f
sz_pwr      = (ARGV.shift || 6    ).to_i
rsz_scale   = (ARGV.shift || 4.0  ).to_f
pct_water   = (ARGV.shift || 0.25 ).to_f
tolerance   = (ARGV.shift || 0.05 ).to_f

puts "Commencing worldgen with parameters #{entropy}, #{granularity} and scale #{sz_pwr}"
puts "="*75
if granularity > 0.8
	puts "Warning, abnormally high granularity parameter, expected the unexpected."
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
ocean    = mda(max_size, max_size)
rivers = mda(max_size, max_size)

puts "Generating bedrock layer"
genTerrain(2, max_size, entropy, granularity+0.2, bedrock)
bImage = genImage(rsz_scale, bedrock, nil)

puts "="*75
puts "Generating hard rock layer"
genTerrain(2, max_size, entropy, granularity, hardrock)
hImage = genImage(rsz_scale, hardrock, nil)

puts "="*75
puts "Generating soft rock layer"
genTerrain(2, max_size, entropy, granularity, softrock)
rImage = genImage(rsz_scale, softrock, nil)

puts "="*75
puts "Generating sedimentary layer"
genTerrain(2, max_size, entropy, granularity+0.1, sediment)
sImage = genImage(rsz_scale, sediment, nil)

puts "="*75
puts "Combining layers..."
sampleZLayers(bedrock, hardrock, softrock, sediment)

#puts "="*75
#puts "Generating oceans..."
#genOceans(max_size, pct_water, tolerance, world, ocean)
#wImage = genImage(rsz_scale, world, ocean)

#puts "="*75
#puts "Simulating river formation and erosion..."
#simRainfall(1000, world, rivers)
#riverImage = genImage(rsz_scale, world, rivers)

makeImages = true
if makeImages
	puts "="*75
	puts "Generating composite image..."
	iSize=max_size*rsz_scale
	fullImage = Image.new(iSize*2+2, iSize*3+3)
	fullImage.store_pixels(0,       0,           iSize, iSize, bImage.get_pixels(0,0,iSize,iSize))
	fullImage.store_pixels(iSize+1, 0,           iSize, iSize, hImage.get_pixels(0,0,iSize,iSize))
	fullImage.store_pixels(0,       iSize+1,     iSize, iSize, rImage.get_pixels(0,0,iSize,iSize))
	fullImage.store_pixels(iSize+1, iSize+1,     iSize, iSize, sImage.get_pixels(0,0,iSize,iSize))
#	fullImage.store_pixels(0,       (iSize+1)*2, iSize, iSize, wImage.get_pixels(0,0,iSize,iSize))
#	fullImage.store_pixels(iSize+1, (iSize+1)*2, iSize, iSize, riverImage.get_pixels(0,0,iSize,iSize))
	fullImage.display
end

exit
