#!/usr/bin/ruby
require 'rubygems'
require 'RMagick'
include Magick

# Place initial values in our starting square so the methods have something to average
def seedIndices(initial, size)
	seed = File.read('/dev/urandom', 1)
	seed = Time.now.to_i
	srand(seed.to_i)
	putIndex(0,      0,      initial)
	putIndex(0,      size-1, initial)
	putIndex(size-1, 0,      initial)
	putIndex(size-1, size-1, initial)
end

# The following two functions are to provide a generic interface to the array used to store the heightmap data
# This way, the script can be easily modified to output to a different format
def getIndex(x, y)
	view = $img.get_pixels(x,y,1,1)
	pixel = view[0]
	scaled = pixel.blue 
	mag = scaled / $scale
	return mag
end

def putIndex(x, y, value)
	scaled = value * $scale
	if scaled > $max_value || scaled < 0
		puts "Warning: value exceeds range [0,#{$max_value}], data will be lost"
	end
	pixel = Pixel.new
	pixel.red = scaled.to_i
	pixel.green = scaled.to_i
	pixel.blue = scaled.to_i
	$img.store_pixels(x,y,1,1,[pixel])
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
	end

	# The highest level will be computed first, with 1 segment
	segments = (size - 1) / level
	puts "Computing #{segments} segments for level #{level}"

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

def genTerrain(power, initial, entropy, granularity)
	size = 2 ** power + 1
	seedIndices(initial, size)
	genPass(2, size, entropy, granularity)
end

def colorizeTerrain(size, cutoff1, cutoff2)
	puts "Coloring terrain based on heights #{cutoff1} and #{cutoff2}"
	pixels = $img.get_pixels(0,0,size,size)
	pixels.each do |pixel|
		if pixel.blue > cutoff1
			pixel.red = 0
			pixel.green = 0
		elsif pixel.blue > cutoff2
			pixel.blue = 0
			pixel.red = 0
		else
			pixel.blue = 0
			pixel.green = 0
		end
	end
	$img.store_pixels(0,0,size,size,pixels)
end

# Simulate light from the northwest at a 45 degree angle
def computeShadowMap(img, size, filename)
	shadowMap = Image.new(size,size)
	shadowMap = img.copy
	shadowMap.view(0,0,size,size) do |sMap|
	img.view(0,0,size,size) do |iArr|
		size.times do |x|
			size.times do |y|
				height = iArr[x][y].blue
				sX = x+1
				sY = y+1
				until (sX >= size || sY >= size)
					height = height - 1
					nHeight = iArr[sX][sY].blue

					if (height <= 0 || iArr[sX][sY].blue >= height)
						break
					else
						sMap[sX][sY].blue = 128
						sMap[sX][sY].green = 0
						sMap[sX][sY].red = 0
					end
					
					sX = sX + 1
					sY = sY + 1
				end
			end
		end
	end
	end

	shadowMap.resize!(size*10, size*10, filter=BoxFilter, support=1.0)
	shadowMap.write(filename)
end

entropy=1
entropy=1
entropy=1
entropy=1
granularity=0.5
max_pwr=6

highAltitude=180
waterLevel=70

if ARGV.length >=2 
	entropy = ARGV[0].to_f
	granularity = ARGV[1].to_f
end

if ARGV.length >=3
	max_pwr=ARGV[2].to_i
end

if ARGV.length >=5
	highAltitude = ARGV[3].to_i
	waterLevel = ARGV[4].to_i
end

$scale=1
$max_value=255
max_size=2**max_pwr + 1

$img = Image.new(max_size, max_size)

genTerrain(max_pwr, 128, entropy, granularity)

cTime = Time.now.to_i
standard = $img.resize(max_size*10, max_size*10, filter=BoxFilter, support=1.0)
standard.write("#{cTime}.jpeg")
computeShadowMap($img, max_size, "#{cTime}_shadow.jpeg")
colorizeTerrain(max_size, highAltitude, waterLevel)
colored = $img.resize(max_size*10, max_size*10, filter=BoxFilter, support=1.0)
colored.write("#{cTime}_color.jpeg")

colored.display

exit
