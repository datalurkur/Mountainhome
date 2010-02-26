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
  end # def initialize

  def setLayers(layers)
    @layers = layers
  end

  # Examine the world layers and populate the map with appropriate tiles
  def getHighest(slice)
    len = slice.length - 1
    (0...len).each do |index|
      rInd = len-index
      return index if slice[rInd] > slice[rInd-1]
    end
    return 0
  end

  def populate(scene)
    # Set the scene that we are populating.
    self.scene = scene

    # Currently, this only generates a very top layer of tiles
    #  This will need to be more complex later, when concave geometry exists under the surface of the map
    # In fact, this is totally a hack.  This needs to be rewritten later
    types = [Bedrock, Hardrock, Softrock, Sediment]

    # Make a quick pass over the map, adding a tile for each [x,y] pair
    @layers.each_with_index do |row,x|
      row.each_with_index do |col,y|
        index = getHighest(col)
        z=col[index]
        puts "Placeholding #{[x,y,z.to_i].inspect}"
        # Put a placeholder for now, replace with an actual tile later
        @tiles[x][y][z.to_i] = index
      end # row.each_with_index
    end # @layers.each_with_index
  
    # For each instantiated tile, fill in tiles below it to avoid leaving holes in the surface
    @tiles.each_with_index do |rows,x|
      rows.each_with_index do |cols,y|
        cols.each_with_index do |node,z|
          if node and node.class == Fixnum
            # Create a short array of nearest neighbors
            neighbors = []
            neighbors << [x-1,y] if (x-1)>=0
            neighbors << [x,y-1] if (y-1)>=0
            neighbors << [x+1,y] if (x+1)<@width
            neighbors << [x,y+1] if (y+1)<@height

            # Work our way down and fill in tiles below this one as needed
            #  This is so we don't wind up with vertical holes in the map
            # Keep track of which neighors are occupied
            occ_track = Array.new(neighbors.length, false)
            #occ_track = neighbors.collect { |n| not (@tiles[n[0]][n[1]][z]==nil) }
            (-1..z).each do |step_depth|
              # Step downward along the z-axis
              current_depth = z - step_depth
              next if current_depth >= @depth

              # Check our new, lower neighbors to see if they're occupied
              neighbors.each_with_index do |n,ind|
                (occ_track[ind] = true) if ((not occ_track[ind]) and @tiles[n[0]][n[1]][current_depth])
              end
              # Count the number of occupied neighbors and break if there are no holes
              filled = occ_track.inject(0) { |val,dat| dat ? val+1 : val }
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
              tile = types[index].new("#{[x,y,current_depth].inspect}", self, "tile_wall", "red")
              tile.set_position(x,y,current_depth)
              @tiles[x][y][current_depth] = tile
            end # (-1..z).each do
          end # if node and node.class == Fixnum
        end # cols.each_with_index
      end # rows.each_with_index
    end # @tiles.each_with_index

    @tiles.each_with_index do |rows, x|
      rows.each_with_index do |cols, y|
        cols.each_with_index do |node, z|
          if node and node.class == Fixnum
            # We may need to rotate the mesh, depending on the configuration of its neighbors
            rot = 0 # 0 is the default of "no rotation"
            # If this is the highest possible layer, there obviously won't be any corners up
            if (z+1) < @depth
              # Create a short array of nearest neighbors
              neighbors = []
              neighbors << [x-1, y,    90] if (x-1)>=0
              neighbors << [x,   y-1, 180] if (y-1)>=0
              neighbors << [x+1, y,   270] if (x+1)<@width
              neighbors << [x,   y+1,   0] if (y+1)<@height

              #occ_track = neighbors.collect { |n| not (@tiles[n[0]][n[1]][z+1]==nil) }
              #occupied = occ_track.inject(0) { |val,dat| dat ? val + 1 : val }
              occupied = []
              neighbors.each_with_index { |n,nind| occupied << nind if @tiles[n[0]][n[1]][z+1]!=nil }

              # Determine the geometry of the current tile, as well as its rotation
              case occupied.length
              when 0 then geo = "tile_noneup"
              when 1 
                geo = "tile_oneup"
                rot = neighbors[occupied[0]][2]
              when 2 
                if (occupied[1]-occupied[0]).even?
                  geo = "tile_twooppup"
                  rot = neighbors[occupied[1]][2]
                else
                  geo = "tile_twoadjup"
                  if not (occupied[0]==0 and occupied[1]==3)
                    rot = neighbors[occupied[0]][2]
                  end
                end
              when 3 
                indices = (0...neighbors.length).to_a
                missing = indices-occupied
                if missing.length <= 0
                  geo = "tile_allup"
                else
                  geo = "tile_threeup"
                  rot = neighbors[missing[0]][2]
                end
              when 4 then geo = "tile_allup"
              else geo = "ERROR" # This shouldn't be hit...ever.
              end
            else
              geo = "tile_noneup"
            end

            puts "Computed geometry type: #{geo}"

            # Create the tile and replace the dummy "true" node with the actual tile
            tile = types[node].new("#{[x,y,z].inspect}", self, geo, "white")
            #tile = types[node].new("#{[x,y,z].inspect}", self, "tile_allup", "white")
            tile.set_position(x,y,z)
            tile.rotate(rot,0,0,1)
            @tiles[x][y][z] = tile
          end # if node and node.class
        end # cols.each_with_index
      end # rows.each_with_index
    end # @tiles.each_with_index
  end # def populate
end
