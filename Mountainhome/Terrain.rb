class Terrain < MHTerrain
    # This is just a unit test, basically
    def test_populate
        seed = rand(100000)
        srand(seed)
        num_points = 1000
        value_range = 1000
    
        $logger.info "Perfoming sanity check on terrain with seed #{seed}"
    
        # Generate a whole bunch of random test points
        test_points = []
        test_values = []
        num_points.times do
            new_pt  = [rand(self.width), rand(self.height), rand(self.depth)]
            new_val = rand(value_range)
            if not test_points.include? new_pt
                test_points << new_pt
                test_values << new_val
            end
        end
        
        # Set values
        test_points.each_with_index { |pt,ind| set_tile(pt[0], pt[1], pt[2], test_values[ind]) }
        
        # Verify that values emerge the same as when they went in
        test_points.each_with_index do |pt, ind|
            ret_val = get_tile(pt[0], pt[1], pt[2])
            if ret_val != test_values[ind]
                $logger.info "****FAILURE in terrain test for point #{pt}"
            end
        end
    end

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
