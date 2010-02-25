#!../ruby/bin/ruby

require 'MountainHomeDSL.rb'

def linInterp(array, value)
  array.each_with_index { |a, i| return i if value < a }
  return array.length
end

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

	# Make a quick pass over the map, adding a tile for each [x,y] pair
    @layers.each_with_index do |row,x|
      row.each_with_index do |col,y|
	index = getHighest(col)
        z=col[index]
	puts "Populating #{[x,y,z.to_i].inspect}"
	# Put a placeholder for now, replace with an actual tile later
        @tiles[x][y][z.to_i] = index
      end
    end
	
	# For each instantiated tile, fill in tiles below it to avoid leaving holes in the surface
	@tiles.each_with_index do |rows,x|
	  rows.each_with_index do |cols,y|
	    cols.each_with_index do |node,z|
		  if node and node.class == Fixnum
			puts "Found a fixnum @ #{[x,y,z].inspect}"
			# Create a short array of nearest neighbors
			neighbors = []
			neighbors << [x-1,y] if (x-1)>=0
			neighbors << [x,y-1] if (y-1)>=0
			neighbors << [x+1,y] if (x+1)<@width
			neighbors << [x,y+1] if (y+1)<@height
			puts "Neighbors: #{neighbors.inspect}"

			# Work our way down and fill in tiles below this one as needed
			#  This is so we don't wind up with vertical holes in the map
			# Keep track of which neighors are occupied
			occ_track = Array.new(neighbors.length, false)
			#occ_track = neighbors.collect { |n| not (@tiles[n[0]][n[1]][z]==nil) }
			(-1..z).each do |step_depth|
				# Step downward along the z-axis
				current_depth = z - step_depth
				puts "=step depth=#{current_depth}"
				next if current_depth >= @depth

				# Check our new, lower neighbors to see if they're occupied
				neighbors.each_with_index do |n,ind|
					(occ_track[ind] = true) if ((not occ_track[ind]) and @tiles[n[0]][n[1]][current_depth])
				end
				puts "Occupied @ level #{current_depth}: #{occ_track.inspect}"
				# Count the number of occupied neighbors and break if there are no holes
				filled = occ_track.inject(0) { |val,dat| dat ? val+1 : val }
				puts "Filled: #{filled}"
				break if filled >= occ_track.length
				next if current_depth >= z

				# If every side has (or had) neighbors, break out early
				# Otherwise, fill in a new tile here
				# FIXME - remove this sanity check later on when this is proven to work
				if @tiles[x][y][current_depth]
					puts "ERROR!  Tile already exists!  Oh fu-"
				end
				puts "Adding sub-tile #{[x,y,current_depth].inspect}"
				index = linInterp(@layers[x][y], current_depth)
				tile = types[index].new("#{[x,y,current_depth].inspect}", self, "tile_allup", "red")
				tile.set_position(x,y,current_depth)
				@tiles[x][y][current_depth] = tile
				puts "Done!"
			end
		  end
		end
	  end
	end

	@tiles.each_with_index do |rows, x|
		rows.each_with_index do |cols, y|
			cols.each_with_index do |node, z|
				if node and node.class == Fixnum
					if (z+1) < @depth
					# Create a short array of nearest neighbors
					neighbors = []
					neighbors << [x-1,y] if (x-1)>=0
					neighbors << [x,y-1] if (y-1)>=0
					neighbors << [x+1,y] if (x+1)<@width
					neighbors << [x,y+1] if (y+1)<@height

					occ_track = neighbors.collect { |n| not (@tiles[n[0]][n[1]][z+1]==nil) }
					occupied = occ_track.inject(0) { |val,dat| dat ? val + 1 : val }

					# Determine the geometry of the current tile
					case occupied
					when 0 then geo = "tile_noneup"
					when 1 then geo = "tile_oneup"
					when 2 then geo = "tile_twoadjup" # FIXME
					when 3 then geo = "tile_threeup"
					when 4 then geo = "tile_allup"
					else geo = "ERROR" # This shouldn't be hit...ever.
					end
					puts "Computed geometry type: #{geo}"

					# Create the tile and replace the dummy "true" node with the actual tile
					if @tiles[x][y][z].class != Fixnum
						puts "ABLWABLWABL ERRROR - #{node.class}"
					end
					#tile = types[node].new("#{[x,y,z].inspect}", self, geo, "white")
					tile = types[node].new("#{[x,y,z].inspect}", self, "tile_noneup", "white")
					tile.set_position(x,y,z)
					@tiles[x][y][z] = tile
				end # if node and node.class
			end # cols.each_with_index
		end # rows.each_with_index
	end # @tiles.each_with_index
  end # def populate

  end
  #def setTile(x,y,z,tile)
  #  @tiles[x][y][z] = tile
  #end

  #def getTile(x,y,z)
  #  @tiles[x][y][z]
  #end
end
