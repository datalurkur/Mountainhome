#!/usr/local/bin/ruby
require './test_harness'

require 'rubygems'
require 'RMagick'
include Magick

require '../../Mountainhome/HeightMap'
#require '../../Mountainhome/TerrainBuilder'

def time(&block)
    t_s = Time.now
    yield if block_given?
    t_f = Time.now
    puts "Block executed in #{t_f - t_s}"
end

=begin
def perf_test
    (7..11).collect { |dim| 2**dim+1}.each do |size|
        puts "====================================="
        puts "Generating heighmaps for size #{size}"
        puts "Generating old heightmap"
        time {
            h = HeightMap.midpoint(size, 5000.0, 0.55)
        }
        puts "Generating new heightmap"
        time {
            MidPoint.build(:size => [size,size], :scaling => 0.55)
        }
    end
end
=end

def image_test
    hmap = MidPoint.build(:power => 9, :scaling => 0.4)
    hmap.extend :bottom
    hmap.clamp_to! (0..255)
    build_image_for(hmap, "extended_heightmap")
end

def build_image_for(hmap, name="test")
    image = Image.new(hmap.size.x, hmap.size.y)
    pixels = Array.new(hmap.size.x*hmap.size.y)

    hmap.size.x.times do |x|
        hmap.size.y.times do |y|
            pixels[y*hmap.size.x+x]=stratefied_pixel_at(hmap.data[x][y])
        end
    end

    image.store_pixels(0, 0, hmap.size.x, hmap.size.y, pixels)
    image.write("#{name}.jpg")
end

def pixel_at(value)
    p = Pixel.new
    p.red = value
    p.green = 0.0
    p.blue = 0.0
    p
end

def stratefied_pixel_at(value)
    p = Pixel.new
    if value < 85
        p.red = (value / 10).to_i * 25
        p.blue = 0
        p.green = 0
    elsif value < 170
        p.red = 0
        p.blue = ((value - 85) / 10).to_i * 25
        p.green = 0
    else
        p.red = 0
        p.blue = 0
        p.green = ((value - 170) / 10).to_i * 25
    end
    p
end

#perf_test
image_test
