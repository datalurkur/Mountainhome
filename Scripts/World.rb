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

    super(@width, @height, @depth)
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

  def populate()
    # Currently, this only generates a very top layer of tiles
    #  This will need to be more complex later, when concave geometry exists under the surface of the map
    # In fact, this is totally a hack.  This needs to be rewritten later
    types = [Bedrock, Hardrock, Softrock, Sediment]

    # Make a quick pass over the map, adding the highest tile for each [x,y] pair
    @layers.each_with_index do |row,x|
      row.each_with_index do |col,y|
        index = getHighest(col)
        z=col[index]
        # Put a placeholder for now, replace with an actual tile later
        @tiles[x][y][z.to_i] = index
      end # row.each_with_index
    end # @layers.each_with_index

    # To ensure tiles are assigned the correct geometry later, make sure no tiles depend on this [x,y] for state
    # For each [x,y] pair, start at z-level 0 and move upwards until we hit a tile, filling any gap with dependent neighbors along the way
    @tiles.each_with_index do |rows,x|
      rows.each_with_index do |cols,y|
        fill = false
        cols.each_with_index do |node,z|
          # If we've hit the surface tile, move on to the next [x,y] pair
          break if node and node.class == Fixnum
          if not fill
            # Perform operations for  bounds checking
            #  to avoid redundant operations later
            xlower = ((x-1)>=0)
            ylower = ((y-1)>=0)
            xupper = ((x+1)<@width)
            yupper = ((y+1)<@height)

            # Prepare coordinates of the neighbors
            # The last element is a tile's eligibility (whether it's out of bounds)
            neighbors = []
            neighbors << [x-1,y-1,(xlower and ylower)]
            neighbors << [x-1,y  ,(xlower)           ]
            neighbors << [x-1,y+1,(xlower and yupper)]
            neighbors << [x  ,y-1,           (ylower)]
            neighbors << [x  ,y+1,           (yupper)]
            neighbors << [x+1,y+1,(xupper and yupper)]
            neighbors << [x+1,y  ,(xupper)           ] 
            neighbors << [x+1,y-1,(xupper and ylower)] 

            # Check to see if any of this tile's neighbors are dependant
            neighbors.each_with_index do |neighbor, index|
              # If this neighborbor is filled, make a new tile and move to the next z-level
              if not neighbor[2]
                next
              elsif @tiles[neighbor[0]][neighbor[1]][z] and @tiles[neighbor[0]][neighbor[1]][z].class == Fixnum
                fill = true
                break
              end
            end # neighbors.each_with_index
          end
          if fill
            tiletype = linInterp(@layers[x][y],z)
            tile = types[tiletype].new("#{[x,y,z].inspect}", self, "tile_wall", "red")
            tile.set_position(x,y,z)
            @tiles[x][y][z] = tile
          end
        end # cols.each_with_index
      end # rows.each_with_index
    end # @tiles.each_with_index

    @tiles.each_with_index do |rows, x|
      rows.each_with_index do |cols, y|
        cols.each_with_index do |node, z|
          # If this tile contains a fixnum, that means it's a surface tile
          if node and node.class == Fixnum
            # Prepare for bounds checking
            xlower = ((x-1)>=0)
            ylower = ((y-1)>=0)
            xupper = ((x+1)<@width)
            yupper = ((y+1)<@height)
            
            # Prepare coordinates of the neighbors
            neighbors = []
            neighbors << [x-1,y-1,(xlower and ylower)] # 0 SW
            neighbors << [x-1,y  ,(xlower)           ] # 1 W
            neighbors << [x-1,y+1,(xlower and yupper)] # 2 NW
            neighbors << [x,  y-1,           (ylower)] # 3 S
            neighbors << [x,  y+1,           (yupper)] # 4 N
            neighbors << [x+1,y+1,(xupper and yupper)] # 5 NE
            neighbors << [x+1,y  ,(xupper)           ] # 6 E
            neighbors << [x+1,y-1,(xupper and ylower)] # 7 SE

            corner_indices = []
            corner_indices << [0,1,3]
            corner_indices << [1,2,4]
            corner_indices << [4,5,6]
            corner_indices << [3,6,7]

            corners_raised = []
            corner_indices.each_with_index do |ind_arr, corner_index|
              is_raised = true
              ind_arr.each do |index|
                this_corner = neighbors[index]
                is_raised = false if this_corner[2] and (not @tiles[this_corner[0]][this_corner[1]][z])
              end
              (corners_raised << corner_index) if is_raised
            end

            # Set the geometry and rotation of the tile
            geo = ""
            rotation = 0
            case corners_raised.length
            when 0
              # This tile is noise and should be culled.  Make sure the one beneath it is occupied
              if z!=0 and @tiles[x][y][z-1]==nil
                tile = types[node].new("#{[x,y,z-1].inspect}",self,"tile_wall","blue")
                tile.set_position(x,y,z-1)
                @tiles[x][y][z-1]=tile
              end
              next
            when 1
              rotation = (corners_raised[0] * 90)
              geo = "tile_convex_corner"
            when 2
              if (corners_raised[1] - corners_raised[0]).even?
                geo = "tile_pinch"
                if corners_raised[0] == 0
                  rotation = 90
                end
              else
                if ((corners_raised[0] == 0) and (corners_raised[1] == 3))
                  rotation = 270
                else
                  rotation = (corners_raised[0] * 90)
                end
                geo = "tile_ramp"
              end
            when 3
              missing = (0...corner_indices.length).to_a - corners_raised
              rotation = (missing[0] * 90)
              geo = "tile_concave_corner"
            when 4
              geo = "tile_wall"
            else
              $logger.error "Unhandled corner case hit - a node with more than four corners wow!"
            end

            # Create the tile and replace the dummy "true" node with the actual tile
            tile = types[node].new("#{[x,y,z].inspect}", self, geo, "white")
            tile.set_position(x,y,z)
            tile.rotate(rotation,0,0,-1)
            @tiles[x][y][z] = tile
          end # if node and node.class == Fixnum
        end # cols.each_with_index
      end # rows.each_with_index
    end # @tiles.each_with_index
  end # def populate
end
