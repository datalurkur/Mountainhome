#!../ruby/bin/ruby

require 'MountainHomeDSL.rb'

class MountainhomeWorld < MHWorld
  def initialize(params={})
    @height = params[:height] || 33
    @width  = params[:width]  || 33
    @depth  = params[:depth]  || 17

    @layers = []
    @tiles  = Array.new(@width) { Array.new(@height) { Array.new(@depth, nil) } }

	super()
  end

  def setLayers(layers)
    @layers = layers
  end

  # Examine the world layers and populate the map with appropriate tiles
  def getHighest(slice)
    len = slice.length - 1
    (0...len).each do |index|
	rInd = len-index
	if slice[rInd] > slice[rInd-1]
          return index
        end
    end
    return 0
  end

  def populate
    # Currently, this only generates a very top layer of tiles
    #  This will need to be more complex later, when concave geometry exists under the surface of the map
    # In fact, this is totally a hack.  This needs to be rewritten later
	types = [Bedrock, Hardrock, Softrock, Sediment]
    @brent = []
    @layers.each_with_index do |row,x|
      row.each_with_index do |slice,y|
		$logger.info "Populating node #{[x,y].inspect}"
	    index = getHighest(slice)
		z=slice[index]
	    tile = types[index].new("#{[x,y].inspect}", self, "tile", "white")
		tile.set_position(x,y,z)
        @tiles[x][y][z.to_i] = tile
      end
    end
  end

  def setTile(x,y,z,tile)
    @tiles[x][y][z] = tile
  end

  def getTile(x,y,z)
    @tiles[x][y][z]
  end
end
