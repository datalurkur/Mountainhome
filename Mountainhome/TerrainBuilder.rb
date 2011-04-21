require 'HeightMap'

class TerrainBuilder
    def self.form_strata(world, layers=[], openness=0.2, volcanism=0.5)
        # NOTE - Eventually, I expect that terrain types will have information that dictates how they form
        #  This means that eventually this method will do more complicated work based on the paramters for
        #  each material type
        # NOTE - Volcanism is a variable that increases the chances of things like seismic activity and magma
        stack   = HeightMapStack.new(:size => [world.width, world.height, world.depth])
        fill_to = (world.depth * (1.0 - openness)).to_i
        shears  = 0

        # Playing around with some different ways of forming the world, this one additive
        # Become less entropic as we move upwards
        entropy_range = [0.40, 0.65]
        layer_entropy = entropy_range.min
        entropy_step  = (entropy_range.max - entropy_range.min) / layers.size

        # Shear less and less as we get higher up
        shear_range = [10, 2]
        shear_size  = shear_range.max
        shear_step  = -((shear_range.max - shear_range.min) / layers.size).to_i

        layers.each_with_index do |layer,i|
            previous_maximum = stack.maximum

            # Determine scaling for this layer
            thickness_left = fill_to - previous_maximum
            scaling = (i+1.0) / (i+2.0)
            this_thickness = (thickness_left * scaling).to_i

            $logger.info "Adding layer #{layer} with thickness #{this_thickness}"
            stack.add_layer(layer, layer_entropy, ((this_thickness*0.25)...this_thickness))
            if(rand() < volcanism)
                stack.shear(shear_size, 1, 1)
                shears += 1
            end
            shear_size    += shear_step
            layer_entropy += entropy_step
        end

        stack.sample_to_world(world)
    end

    def self.form_tunnel(world, tunnel_size=3, tunnel_wander=0.4, irregularity=0.5, damping=0.4, damping_power=2, type=nil)
        # Roll random vectors to determine starting directions and position
        position  = [rand(world.width), rand(world.height)]
        surface = world.terrain.get_surface(*position)
        position << rand(surface)

        direction = [rand-0.5, rand-0.5, rand-0.5]
        direction = direction.normalize

        # Traverse in both directions away from the starting position
        [1, -1].each do |direction_mod|
            damping_factor = 1.0
            dir = direction.collect { |i| i * direction_mod }
            pos = position.dup

            until (brush_size = tunnel_size * damping_factor) <= 0.5
                # Compute the spherical brush and reject any tiles that are out of bounds
                brush = compute_spherical_batch(brush_size, irregularity, pos)
                brush.reject! { |i| world.out_of_bounds?(*i) }

                # Batch set the brush to the specified tunnel tile (empty, in this case)
                if type.nil?
                    batch_set_type(world, nil, brush)
                else
                    batch_set_type(world, type, brush)
                end

                # Move the position along the directional axis
                pos.each_index { |i| pos[i] = pos[i] + dir[i] }
                break if world.out_of_bounds?(*pos)

                # Modify the direction
                dir.each_index { |i| dir[i] += ((rand - 0.5) * tunnel_wander) }
                dir = dir.normalize

                # Shrink the size of the tunnel by some damping factor
                damping_factor *= 1.0 - ((rand * damping) ** damping_power)
            end
        end
    end

    # ========================
    # TERRAIN ADDITION METHODS
    # ========================
    def self.average(world, passes = 1)
        $logger.info "Averaging terrain: passes #{passes}"
        passes.times do |pass|
            $logger.info "Performing pass #{pass}"
            vals = []
            world.width.times do |x|
                world.height.times do |y|
                    z = world.get_surface(x,y)
                    vals[x] ||= []
                    vals[x][y] = z
                end
            end
            world.width.times do |x|
                world.height.times do |y|
                    z = vals[x][y]
                    next if z < 0
                    thisType = world.get_tile_type(x, y, z)

                    window_width = 3
                    max_radius = (2*(window_width**2))**0.5

                    sum    = window_width * vals[x][y]
                    weight = window_width

                    (x-window_width..x+window_width).each do |local_x|
                        next if local_x < 0 || local_x >= world.width
                        (y-window_width..y+window_width).each do |local_y|
                            next if local_y < 0 || local_y >= world.height || (local_x == x && local_y == y)

                            radius = ((local_x - x)**2 + (local_y - y)**2)**0.5
                            thisWeight = max_radius / radius

                            sum += thisWeight * vals[local_x][local_y]
                            weight += thisWeight
                        end
                    end

                    newVal = (sum / weight).to_i

                    while newVal != z
                        if newVal > z
                            world.set_tile_type(x, y, z+1, thisType)
                            z += 1
                        else
                            world.set_tile_type(x, y, z, nil)
                            z -= 1
                        end
                    end
                end
            end
        end
    end

    def self.generate_riverbeds(world, num_sources=1)
        $logger.info "Generating riverbeds"

        # Generate seed points to build rivers off of
        seed_points  = Array.new(num_sources) { [rand(world.width), rand(world.height)] }
        seed_points.collect! { |point| point << world.get_surface(*point) }
        river_points = []

        # For each seed point, path both upwards and downwards
        seed_points.each do |seed_origin|
            $logger.info "Computing river path for seed point #{seed_origin}"
            river_path = [seed_origin]

            [:ascending, :descending].each do |cardinality|
                position = seed_origin
                while true
                    neighbors = []
                    (position[0]-1..position[0]+1).each do |x|
                        (position[1]-1..position[1]+1).each do |y|
                            next if x==0 && y==0
                            next if world.out_of_bounds?(x, y, 0)
                            neighbors << [x,y,world.get_surface(x,y)]
                        end
                    end
                    # Ensure we don't traverse the same tile twice
                    neighbors.reject! { |n| river_path.include?(n) }
                    break if neighbors.size == 0

                    # Sort the neighboring tiles by height, keeping only the ties for the extreme
                    # Break if we've found the minima or maxima
                    if cardinality == :descending
                        neighbors.sort! { |x,y| x[2] <=> y[2] }
                        minima = neighbors.first
                        neighbors.reject! { |n| n[2] > minima[2] }
                        break if neighbors.first[2] > position[2]
                    else
                        neighbors.sort! { |x,y| y[2] <=> x[2] }
                        maxima = neighbors.first
                        neighbors.reject! { |n| n[2] < maxima[2] }
                        break if neighbors.first[2] < position[2]
                    end
                    # Move a random direction amongst the extrema
                    position = neighbors[rand(neighbors.size)]

                    # Add the current position to the river path
                    river_path << position.dup
                end
            end

            # Add this river path to the river system, weighing each point by how many rivers pass through it
            $logger.info "River runs across #{river_path.size} points"
            river_path.each do |point|
                existing_point = river_points.index(point)
                if existing_point.nil?
                    river_points << [point, 1]
                else
                    river_points[existing_point].last += 1
                end
            end
        end

        # Iterate over the river points, queueing up points to be eroded
        erode_points = []
        river_points.each do |point, weight|
            # Erode the landscape at each point the river passes through using a tapered window
            # This has the effect of rolling a (rough) sphere across the landscape, making a dent as it rolls
            # The size of our window is determined by the breadth of the river (how many river seeds pass through this point)
            erode_points.concat(compute_spherical_batch(weight, 0.0, point))
        end

        erode_points.uniq!
        erode_points.reject! { |point| world.out_of_bounds?(*point) }
        erode_points.each { |point| world.set_tile_type(*point,nil) }
    end

    def self.fill_ocean(world, liquid_klass)
        # Take an average of the landscape height
        average_height = 0
        (0...world.width).each do |x|
            (0...world.height).each do |y|
                average_height += world.get_surface(x,y)
            end
        end
        average_height /= (world.width * world.height)
        $logger.info "Filling oceans to depth #{average_height}"

        # Fill water to this level
        # TODO - Add some variation here to make this builder step more sophisticated and interesting
        (0...world.width).each do |x|
            (0...world.height).each do |y|
                surface_level = world.get_surface(x,y)
                ((surface_level+1)..average_height).each do |z|
                    world.set_tile_type(x,y,z,liquid_klass)
                end
            end
        end
    end

    # =================
    # Utility functions
    # =================

    def self.compute_spherical_batch(dimension, irregularity, offset=[0,0,0])
        radius = dimension.to_i

        results = []
        (-radius..radius).each do |x|
            (-radius..radius).each do |y|
                (-radius..radius).each do |z|
                    if (x**2) + (y**2) + (z**2) <= (radius**2 + (rand * dimension * irregularity))
                        results << [x+offset[0],y+offset[1],z+offset[2]]
                    end
                end
            end
        end
        results
    end

    def self.compute_elliptic_cross_section(dims, offset=[0,0])
        a = (dims[0].to_i ** 2).to_f
        b = (dims[1].to_i ** 2).to_f

        results = []
        (-dims[0]..dims[0]).each do |x|
            (-dims[1]..dims[1]).each do |y|
                # Use the equation for an ellipse to determine if [x,y] is in the cross-section
                if (x**2 / a) + (y**2 / b) <= 1.0
                    results << [x+offset[0],y+offset[1]]
                end
            end
        end
        results
    end

    def self.batch_set_type(world, type, batch)
        batchtype = type.nil? ? nil : type
        batch.each do |dims|
            world.set_tile_type(dims[0], dims[1], dims[2], batchtype)
        end
    end

    # This is just a unit test, basically
    def self.test_populate(world)
        seed = rand(100000)
        srand(seed)
        num_points = 1000
        value_range = 1000

        $logger.info "Perfoming sanity check on terrain with seed #{seed}"

        # Generate a whole bunch of random test points
        test_points = []
        test_values = []
        $logger.info "=Generating test points"
        num_points.times do
            new_pt  = [rand(world.width), rand(world.height), rand(world.depth)]
            new_val = rand(value_range)
            if not test_points.include? new_pt
                test_points << new_pt
                test_values << new_val
            end
        end

        # Set values
        $logger.info "=Seeding test points"
        test_points.each_with_index { |pt,ind| world.set_tile_type(pt[0], pt[1], pt[2], test_values[ind]) }

        # Verify that values emerge the same as when they went in
        $logger.info "=Verifying test points..."
        test_points.each_with_index do |pt, ind|
            ret_val = world.get_tile_type(pt[0], pt[1], pt[2])
            if ret_val != test_values[ind]
                $logger.info "****FAILURE in terrain test for point #{pt}"
            end
        end
        $logger.info "...Done!"
    end
end

# Used to composite many heightmaps together
class HeightMapStack
    def initialize(args={})
        @size            = args[:size]
        @hmaps           = []
        @feature_density = args[:feature_density]
    end

    def out_of_bounds?(coords); out_of_bounds(*coords); end
    def out_of_bounds?(x,y,z)
        return true if x < 0 || x >= @size.x
        return true if y < 0 || y >= @size.y
        return true if z < 0 || z >= @size.z
        return false
    end

    def maximum() (@hmaps.size > 0) ? @hmaps.last[:map].extrema.max : 0 end
    def minimum() (@hmaps.size > 0) ? @hmaps.last[:map].extrema.min : 0 end

    def sample_to_world(world)
        if world.depth < @size.z
            $logger.warning "Map depth exceeds world depth, clipping will occur."
        end
        if world.width != @size.x || world.height != @size.y
            $logger.error "World dimensions do not match map dimensions, unable to sample data to world."
            return
        end

        @size.x.times do |x|
            @size.y.times do |y|
                current_z = 0
                @hmaps.each_with_index do |hmap,i|
                    hmap_z = hmap[:map].data[x][y]
                    (current_z..hmap_z).each do |z|
                        world.set_tile_type(x, y, z, hmap[:klass])
                    end
                    current_z = hmap_z + 1
                end
                (current_z...world.depth).each do |z|
                    world.set_tile_type(x, y, z, nil)
                end
            end
        end
    end

    # Generates a new heightmap and layers it *on top* of any existing terrain
    def add_layer(klass, scaling = 0.4, clamp_range = nil)
        layer = MidPoint.build(:size => @size, :scaling => scaling)

        voronois = Voronois.build(:size => @size, :random_features => @feature_density)
        layer.mix_with(voronois, 0.3)

        # Clamp the values to a specified range and add this layer on top of the previous one
        layer.clamp_to!(clamp_range || (0...@size.z))
        layer.displace_by(@hmaps.last[:map], @size.z-1) if @hmaps.size > 0

        @hmaps << {
            :klass  => klass,
            :map => layer
        }
    end

    # Generates a new heightmap and merges it with existing terrain
    def composite_layer(klass, scaling=0.4, clamp_range=nil)
        layer = MidPoint.build(:size => @size, :scaling => scaling)

        voronois = Voronois.build(:size => @size, :random_features => @feature_density)
        layer.mix_with(voronois, 0.2)

        layer.clamp_to!(clamp_range || (0...@size.z))

        @hmaps << {
            :klass  => klass,
            :map => layer
        }
    end

    def shear(magnitude=5, variation=1, cap)
        # Determine which way the fault will run and which side of the fault will be lowered
        # If start_axis is :x, we'll begin somewhere *on* the X-axis
        start_axis  = (rand(2).to_i == 0) ? :x   : :y
        # :low means we lower [0...point], :high means we lower [point...boundary]
        raised_side = (rand(2).to_i == 0) ? :low : :high

        # Start at a random location at one of the lower edges of the map
        # Initialize our direction vector to point us towards the opposite edge
        case start_axis
            when :x
                start_position = [rand(@size.x), 0]
                direction      = [0,             1]
            when :y
                start_position = [0, rand(@size.y)]
                direction      = [1,             0]
        end

        current_position = start_position

        # Move towards the opposite edge one unit at a time until we get there or go out of bounds
        until out_of_bounds?(current_position[0], current_position[1], 0)
            # STEP 1:
            # Take a slice of the map between our current position and the appropriate edge
            #  as dictated by raised_side and drop it down some number of units equal to the
            #  magnitude of the fault

            # The midpoint of this slice is dependent on our axis of movement
            slice_median = ((start_axis == :x) ? current_position[0] : current_position[1])

            # The range of the slice is dependent on both our axis of movement and the raised side
            slice_range = case raised_side
            when :low;  (0..slice_median)
            when :high; (start_axis == :x) ? (slice_median...@size.x) : (slice_median...@size.y)
            end

            # Iterate over each element in the slice and drop its column by the shear magnitude
            slice_range.each do |slice_elem|
                # Generate the coordinates for this element of the slice orthogonal to our axis of movement
                slice_coords = (start_axis == :x) ?
                    [slice_elem, current_position[1]] :
                    [current_position[0], slice_elem]

                # Drop the column
                @hmaps.each do |hmap|
                    hmap[:map].data[slice_coords[0]][slice_coords[1]] =
                        [hmap[:map].data[slice_coords[0]][slice_coords[1]] - magnitude, 0].max
                end
            end

            # STEP 2:
            # Generate a random directional variation in the range [-variation, variation]
            direction_variation = (rand((variation*2)+1) - variation).to_i

            # Add the directional variation to our current direction vector
            case start_axis
            when :x; direction[0] += direction_variation
            when :y; direction[1] += direction_variation
            end

            # Check to make sure the directional vector doesn't exceed the cap if it's set
            direction.each_with_index do |d,i|
                direction[i] = [[d,-cap].max, cap].min
            end if cap

            # STEP 3:
            # Advance the fault along the direction vector
            current_position[0] += direction[0]
            current_position[1] += direction[1]

            # As a corner case, we need to make sure the fault makes it all the way across the map,
            #  even if it travels along the edge.  This way, the fault doesn't end early and leave a
            #  large portion of the map raised if it hits the raised edge early.
            case start_axis
            when :x; current_position[0] = [[current_position[0], 0].max, @size.x-1].min
            when :y; current_position[1] = [[current_position[1], 0].max, @size.y-1].min
            end
        end
    end
end
