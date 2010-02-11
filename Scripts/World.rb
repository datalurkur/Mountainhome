#!../ruby/bin/ruby

require 'MountainHomeDSL.rb'

class World
  def initialize(params={})
    @height = params[:height] || 33
    @width  = params[:width]  || 33
    @depth  = params[:depth]  || 17

    @tiles  = Array.new(@width) { Array.new(@height) { Array.new(@depth) } }
  end

  def setTile(x,y,z,tile)
    @tiles[x][y][z] = tile
  end

  def getTile(x,y,z)
    @tiles[x][y][z]
  end
end
