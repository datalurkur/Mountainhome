class Terrain < MHTerrain
    def out_of_bounds?(x, y, z)
        !((1..width)  === x) ||
        !((1..height) === y) ||
        !((1..depth)  === z)
    end

    def get_tile(x, y, z)
        if out_of_bounds?(x, y, z)
            $logger.error "Cannot get tile at #{[x,y,z].inspect} - index out of bounds"
            return nil
        end

        super(x, y, z)
    end

    def set_tile(x, y, z, tile)
        if out_of_bounds?(x, y, z)
            $logger.error "Cannot set tile at #{[x,y,z].inspect} - index out of bounds"
            return nil
        end

        super(x, y, z, tile)
    end
end
