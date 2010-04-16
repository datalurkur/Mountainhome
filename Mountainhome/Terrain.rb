class Terrain < MHTerrain
    def out_of_bounds?(x, y, z = 0)
        !((0...self.width ) === x) ||
        !((0...self.height) === y) ||
        !((0...self.depth ) === z)
    end

    def get_tile(x, y, z)
        if out_of_bounds?(x, y, z)
            $logger.error "Cannot get tile at #{[x,y,z].inspect} - index out of bounds [#{width} #{height} #{depth}]"
            return 0 # Default to air.
        end

        super(x, y, z)
    end

    def set_tile(x, y, z, tile)
        if out_of_bounds?(x, y, z)
            $logger.error "Cannot set tile at #{[x,y,z].inspect} - index out of bounds [#{width} #{height} #{depth}]"
        else
            super(x, y, z, tile)
        end
    end
end
