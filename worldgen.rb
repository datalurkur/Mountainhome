#!/usr/bin/ruby
require 'rubygems'
require 'RMagick'
include Magick

def mda(width,height)
	Array.new(width) { Array.new(height) }
end

# Place initial values in our starting square so the methods have something to average
def seedIndices(initial, size)
	seed = Time.now.to_i
	srand(seed.to_i)
	iVals=Array.new(4)
	iVals.collect! { |x| rand(initial)-(initial/2.0) }
	puts "Seeding initial indices with range #{initial} and values #{iVals.inspect}"
	#putIndex(0,      0,      initial)
	#putIndex(0,      size-1, initial)
	#putIndex(size-1, 0,      initial)
	#putIndex(size-1, size-1, initial)
	putIndex(0,      0,      iVals[0])
	putIndex(0,      size-1, iVals[1])
	putIndex(size-1, 0,      iVals[2])
	putIndex(size-1, size-1, iVals[3])
end

# The following two functions are to provide a generic interface to the array used to store the heightmap data
# This way, the script can be easily modified to output to a different format
def getIndex(x, y)
	$terrain[x.to_i][y.to_i]
end

def putIndex(x, y, value)
	if $max < value
		$max = value
	end
	if $min > value
		$min = value
	end
	$terrain[x.to_i][y.to_i] = value
end

def calcBox(lowerX, lowerY, upperX, upperY, entropy)
	# Calculate our halfway points
	offset = (upperX - lowerX) / 2.0
	middleX = lowerX + offset
	middleY = lowerY + offset

	# Compute the size of intersection of the diagonals
	# as the average of the four corners plus a random amount
	middle = getIndex(lowerX, lowerY) + getIndex(lowerX, upperY) + getIndex(upperX, lowerY) + getIndex(upperX, upperY)
	middle = middle / 4.0
	middle = middle + rand(entropy) - (entropy/2.0)
	putIndex(middleX, middleY, middle)
end

def calcDiamond(lowerX, lowerY, upperX, upperY, entropy, size)
	# Compute indices
	offset = (upperX - lowerX) / 2.0
	middleX = lowerX + offset
	middleY = lowerY + offset
	preX = lowerX - offset
	postX = upperX + offset
	preY = lowerY - offset
	postY = upperY + offset
	
	midLeft = midRight = midTop = midBottom = -1

	midLeft = getIndex(lowerX, lowerY) + getIndex(lowerX, upperY) + getIndex(middleX, middleY)
	if preX < 0
		midLeft = midLeft * 4.0 / 3.0
	else
		midLeft = midLeft + getIndex(preX, middleY)
	end
	midLeft = midLeft / 4.0
	midLeft = midLeft + rand(entropy) - (entropy/2.0)
	
	midRight = getIndex(upperX, lowerY) + getIndex(upperX, upperY) + getIndex(middleX, middleY)
	if postX >= size
		midRight = midRight * 4.0 / 3.0
	else
		midRight = midRight + getIndex(postX, middleY)
	end
	midRight = midRight / 4.0
	midRight = midRight + rand(entropy) - (entropy/2.0)
	
	midTop = getIndex(lowerX, upperY) + getIndex(upperX, upperY) + getIndex(middleX, middleY)
	if postY >= size
		midTop = midTop * 4.0 / 3.0
	else
		midTop = midTop + getIndex(middleX, postY)
	end
	midTop = midTop / 4.0
	midTop = midTop + rand(entropy) - (entropy/2.0)
	
	midBottom = getIndex(lowerX, lowerY) + getIndex(upperX, lowerY) + getIndex(middleX, middleY)
	if preY < 0
		midBottom = midBottom * 4.0 / 3.0
	else
		midBottom = midBottom + getIndex(middleX, preY)
	end
	midBottom = midBottom / 4.0
	midBottom = midBottom + rand(entropy) - (entropy/2.0)

	putIndex(lowerX, middleY, midLeft)
	putIndex(upperX, middleY, midRight)
	putIndex(middleX, upperY, midTop)
	putIndex(middleX, lowerY, midBottom)
end

def genPass(level, size, entropy, granularity)
	if level + 1 < size
		genPass(level*2, size, entropy/granularity, granularity)
	else
		seedIndices(entropy/granularity, size)
	end

	# The highest level will be computed first, with 1 segment
	segments = (size - 1) / level
	puts "Computing #{segments} segments for level #{level} with params #{entropy},#{granularity}"

	# First, compute the midpoints of all the sections
	segments.times do |x|
		segments.times do |y|
			# Divide the grid into segments ** 2 pieces
			calcBox(x*level, y*level, (x+1)*level, (y+1)*level, entropy)
		end
	end

	# Then, use the midpoint calculations to compute the halfway heights for edges
	segments.times do |x|
		segments.times do |y|
			calcDiamond(x*level, y*level, (x+1)*level, (y+1)*level, entropy, size)
		end
	end
end

def genTerrain(size, initial, entropy, granularity)
	genPass(2, size, entropy, granularity)
end

def genImage(size, filename)
	image = Image.new(size, size)

	#max = $terrain.collect(&:max).max
	#min = $terrain.collect(&:min).min

	offset = -$min
	scale = 255.0 / ($max - $min)
	
	pixels = Array.new(size**2) 
	(0...size).each do |x|
		(0...size).each do |y|
			val = (getIndex(x, y) + offset) * scale
			
			pixel = Pixel.new
			pixel.red = val.to_i
			pixel.green = val.to_i
			pixel.blue = val.to_i
			pixels[x*size+y]=pixel
		end
	end
	
	image.store_pixels(0,0,size,size,pixels)
#	image.resize!(size*10, size*10, filter=BoxFilter, support=1.0)
	image.write(filename)
	image.display
end

$min = $max = 0

entropy     = (ARGV.shift || 1  ).to_f
granularity = (ARGV.shift || 0.5).to_f
sz_pwr      = (ARGV.shift || 6  ).to_i

max_size=2**sz_pwr + 1
$terrain = mda(max_size, max_size)
genTerrain(max_size, 0, entropy, granularity)

cTime = Time.now.to_i
genImage(max_size, "#{cTime}.jpeg")

exit
