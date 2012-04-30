require 'TerrainBuilder'

# Comment added after adding World.rb to my change.

class Timer
    def start(name)
        @times << [name, Time.now]
    end

    def stop
        @times[-1][1] = Time.now - @times[-1][1]
    end

    def reset
        @times = Array.new
    end

    def to_s
        @times << ["total", @times.transpose[1].inject(0, &:+)]
        width = @times.transpose[0].collect(&:size).max + 1
        "\n" + @times.collect { |name, elapsed| "%-#{width}s #{elapsed}" % ["#{name}:"] }.join("\n")
    end
end

class TerrainVerificationDecorator
    def initialize(world)
        @array = Array.new(world.width) { Array.new(world.height) { Array.new(world.depth) { nil } } }
        @world = world
    end

    def verify
        $logger.info "Verifying world!"
        (@world.height - 1).downto(0) do |y|
            line = []
            (@world.width - 1).downto(0) do |x|
                z = @world.get_surface_level(x, y)
                line << "%2s [%-2s]" % [z, get_backup_surface(x, y)]
            end
            $logger.info line.join(" ")
        end

        (@world.height - 1).downto(0) do |y|
            line = []
            0.upto(@world.width - 1) do |x|
                z = @world.get_surface_level(x, y)
                line << "%2s" % [(z - get_backup_surface(x, y)).to_s]
            end
            $logger.info line.join("      ")
        end
    end

    def get_core(x, y)
        @array[x][y].collect { |value| value || "-" }.join(", ")
    end

    def get_backup_surface(x, y)
        zLevel = -1
        @array[x][y].each_with_index do |type, index|
            zLevel = index if type.to_i > 0
        end
        zLevel
    end

    def set_tile_type(x, y, z, tile)
        @world.set_tile_type(x, y, z, tile)
        @array[x][y][z] = tile.class
    end

    def method_missing(name, *args)
        @terrain.send(name, *args)
    end
end

class World < MHWorld
    attr_reader :builder_fiber, :builder_step
    attr_accessor :actors, :cameras

    def clear_fiber
        @builder_fiber = nil
    end

    def initialize(core, action = :load, args={})
        super(core)

        @actors = Array.new
        @locked_actors = Array.new

        pathing_enabled = args[:enable_pathfinding]
        liquid_enabled = args[:enable_liquid]

        case action
        when :empty
            if false
                width  = 1
                height = 1
                depth  = 1

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new {
                    self.liquid_system.setup
                    self.initialize_pathfinding
                }
            elsif true
                width  = 5
                height = 5
                depth  = 5

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new do
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_type(x, y, 0, Dirt) } }

                    set_tile_type(0,0,1,Dirt)
                    set_tile_type(2,2,1,Dirt)

                    set_tile_type(0,0,2,Rock)
                    set_tile_type(0,1,2,Rock)
                    set_tile_type(1,3,2,Rock)
                    set_tile_type(1,4,2,Rock)
                    set_tile_type(2,4,2,Rock)
                    set_tile_type(3,3,2,Water)

                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_type(x, y, 3, Water) } }

                    set_tile_type(4,4,4,Water)

                    self.liquid_system.setup
                    self.initialize_pathfinding
                end
            else
                width  = 6
                height = 4
                depth  = 2

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new do
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_type(x, y, 0, Rock) } }
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_type(x, y, 1, Rock) } }
                    set_tile_type(3, 3, 1, nil)
                    set_tile_type(3, 2, 1, nil)

                    self.liquid_system.setup
                    self.initialize_pathfinding
                end
            end

            self.terrain.auto_update = true

        when :generate
            width  = args[:width]  || 129
            height = args[:height] || 129
            depth  = args[:depth]  || 65

            self.load_empty(width, height, depth, core)

            # Generate a predictable world to see the effects of turning various terrainbuilder features on and off
            #seed = rand(100000)
            seed = 17216

            # seed = 28476 # A pretty large world
            # seed = 98724 # floating tile bug on 9x9x33
            # seed = 84689 # Tunnel to bottom on medium world.
            # seed = 41354 # A pretty, small world.

            $logger.info "Building terrain with seed #{seed}"
            srand(seed)

            @timer = Timer.new
            @builder_fiber = Fiber.new do
                # Do the actual world generation and benchmark it as we go.
                $logger.info "Starting world generation at #{Time.new}:"
                $logger.indent

                # Some features of the world will occur with varying frequency, such as caves and tunnels
                # For now, we'll base these logarithmically off the terrain size
                terrain_power = 0
                while (counter ||= width; counter > 32)
                    counter /= 2
                    terrain_power += 1
                end
                $logger.info "Terrain has power #{terrain_power}"

                @timer.reset

                # Args: world, strata material types, openness (space to leave from world ceiling), volcanism (chances of shears, etc)
                prepare_builder_step(:form_strata, self, [Rock, Dirt], 0, terrain_power*10)

                $logger.info "Carving #{terrain_power} tunnels."
                terrain_power.times do
                    # Args: world
                    prepare_builder_step(:form_tunnels, self)
                end

                # Args: world, number of passes
                prepare_builder_step(:average, self, 1)

                # Args: world, number of rivers to generate
                prepare_builder_step(:generate_riverbeds, self, 2)

                # Args: world, what to fill the ocean with.
                prepare_builder_step(:fill_ocean, self, Water)

                # Args: world, chance of dirt growing grass.
                prepare_builder_step(:handle_tile_growth, self, 1)

                prepare_builder_step(:final)

                $logger.info "Initializing liquids"
                @timer.start("Liquid Initialization")
                self.liquid_system.setup
                @timer.stop

                $logger.info "Initializing pathfinding."
                @timer.start("Pathfinding Init")
                self.initialize_pathfinding
                @timer.stop

                @timer.to_s.split(/\n/).each { |line| $logger.info line }

                terrain.verify if terrain.respond_to?(:verify)

                $logger.info "World generation finished at #{Time.new}."
                $logger.unindent
            end
        when :load
            # TODO: The load has to be called outside of the fiber for now because the
            # dimensions are set based on the loaded data and the dimensions are ALSO
            # needed for setting up the camera. This means the information needs to be
            # available BEFORE the fiber executes. This is bad, though, because it means
            # we have dead time on the loading screen. This needs to be fixed so we have
            # an immediate jump to the loading screen without any camera exceptions.
            self.load(args[:filename]);
            self.terrain.auto_update    = true
            self.liquid_system.setup
            self.initialize_pathfinding
            @builder_fiber = Fiber.new { }
        end

        # This is smart now and will only do the work when it means something. Leave it
        # on by default.
        self.terrain.poly_reduction = true

        # Setup the cameras
        @cameras = []
        @cameras << self.create_camera("BasicCamera", BasicCamera)
        @cameras << self.create_camera("TopDownCamera", TopCamera)
        @cameras << self.create_camera("FirstPersonCamera", FirstPersonCamera, nil)

        self.active_camera = @cameras[0]

        $logger.info("World size: #{width}x#{height}x#{depth}")
    end

    def cycle_cameras
        self.active_camera = self.cameras.last
        self.cameras.unshift(self.cameras.pop)
    end

    def prepare_builder_step(name, *args)
        # Set the builder step name and yield back to the primary loop.
        @builder_step = name.to_s.humanize
        Fiber.yield

        # After the yeild call returns, do the actual builder step.
        last_step = name == :final

        # Only do this work if we weren't given the final step name.
        if !last_step
            self.terrain.auto_update = false

            @timer.start(name.to_s)
            TerrainBuilder.send(name, *args)
            @timer.stop

            $logger.info("Step finished. Generating geometry.")
            @timer.start("Populate")
            self.populate()
            @timer.stop
        end

        self.terrain.auto_update = last_step
    end

    attr_accessor :pathing_enabled

    def pathfinding_initialized?; @pathfinding_initialized ||= false; end

    def initialize_pathfinding
        return unless self.pathing_enabled

        (0...self.width).each do |x|
            (0...self.height).each do |y|
                self.terrain.each_empty_range(x, y) do |start_z, end_z|
                    # Only mark this tile as pathable if it's above solid ground
                    if solid_ground?(x, y, start_z - 1)
                        self.pathfinder.set_tile_pathable(x, y, start_z)
                    else
                        self.pathfinder.set_tile_open(x, y, start_z)
                    end

                    unless start_z + 1 == self.depth
                        ((start_z + 1)..end_z).each do |z|
                            self.pathfinder.set_tile_open(x, y, z)
                        end
                    end
                end
            end
        end

        @pathfinding_initialized = true
    end

    attr_accessor :liquid_enabled

    def local_neighbors(coords)
        n = [[-1,  1], [ 0,  1], [ 1,  1],
             [-1,  0],           [ 1,  0],
             [-1, -1], [ 0, -1], [ 1, -1]].collect do |local|
            [coords.x + local.x, coords.y + local.y, coords.z]
        end
        n.reject { |i| out_of_bounds?(*i) }
    end

    def set_tile_parameter(x, y, z, param, value)
        tile = self.terrain.get_tile_type(x, y, z)
        if tile && tile.has_parameter?(param)
            self.terrain.set_tile_parameter(x, y, z, param, value)
        end
    end

    def get_tile_parameter(x, y, z, param)
        tile = self.terrain.get_tile_type(x, y, z)
        if tile && tile.has_parameter?(param)
            return self.terrain.get_tile_parameter(x, y, z, param)
        end
        nil
    end

    def select_tile(x, y, z)
        set_tile_parameter(x, y, z, :selected, true)
    end

    def deselect_tile(x, y, z)
        set_tile_parameter(x, y, z, :selected, false)
    end

    # TODO - Register tile events with an event handler system so that we can move all of this code to more appropriate places
    def set_tile_type(x, y, z, tile, timer_offset = 0)
        # FIXME This will just slow things down. Consider moving to C where we can wrap
        # the protection in DEBUG build only macros.
        if !tile.nil? && !tile.kind_of?(Class)
            raise RuntimeError, "Do not use set_tile_type with instances of tile types."
        end

        self.terrain.set_tile_type(x, y, z, tile)

        if tile.nil?
            self.liquid_system.process_vacuum(x, y, z, timer_offset)
        elsif tile.ancestors.include?(LiquidModule)
            self.liquid_system.process_liquid(x, y, z, timer_offset)
        end

        if pathfinding_initialized?
            if tile
                self.pathfinder.set_tile_closed(x, y, z)
                interrupt_pathing(x, y, z)
            else
                if solid_ground?(x, y, z - 1)
                    self.pathfinder.set_tile_pathable(x, y, z)
                    invalidate_blocked_paths
                else
                    self.pathfinder.set_tile_open(x, y, z)
                    fall_check(x, y, z)
                    interrupt_pathing(x, y, z)
                end
            end
            if z + 1 < self.depth && self.get_tile_type(x, y, z + 1).nil?
                if solid_ground?(x, y, z)
                    self.pathfinder.set_tile_pathable(x, y, z + 1)
                    invalidate_blocked_paths
                else
                    self.pathfinder.set_tile_open(x, y, z + 1)
                    fall_check(x, y, z + 1)
                    interrupt_pathing(x, y, z + 1)
                end
            end
        end

        # Empty tiles shouldn't be selected.
        deselect_tile(x, y, z) if tile.nil?
    end

    # Adjust the position of actors pathing through here or nearby.
    # FIXME: This still isn't as robust as I'd like. It just checks whether
    # x,y,z is directly in the path, not whether its removal or addition mucked
    # with a large PF node and interrupts a path through that PF node that didn't
    # involve x,y,z. To do this we'd either have to call into C or add awareness
    # of large PF nodes to Ruby directly.
    def interrupt_pathing(x, y, z)
        return # 
        @actors.each do |actor|
            if actor.respond_to?(:path) && actor.path && actor.path.include?([x, y, z])
                # Handle fractional-position interruption.
                int_pos = actor.position.collect { |i| i.to_i }
                if actor.position != int_pos
                    if pathable?(*int_pos)
                        actor.position = int_pos
                    else
                        $logger.info "Integer position is unpathable from #{actor.to_s}: recalculating from last position."
                        # Force recalculation of the path from the last integer position known.
                        # FIXME: This position assignment can make the dwarf do a large
                        # backtrack if traversing a large node. Really the actor should move
                        # to the *closest traversible* node.
                        original_position = actor.position
                        actor.position = actor.last_position
                        actor.calculate_path(true)
                        actor.position = original_position
                        # Backtrack to the last integer position.
                        if actor.path
                            actor.path = [actor.last_position] + actor.path
                        else
                            actor.path = [actor.last_position]
                        end
                        # Don't calculate the path twice.
                        next
                    end
                end
                # Force recalculation of the path.
                actor.calculate_path(true)
            end
        end
    end

    # Check for actors above or at the tile, and make them 'fall.'
    # TODO: Plants shouldn't fall...
    def fall_check(x, y, z)
        fall_to_z = fall_to_z(x, y, z)
        @actors.each do |actor|
#            $logger.info "doing fall check on #{x}, #{y}, #{z}: #{actor.to_s} (#{actor.position})"
            if actor.position == [x, y, z] || (actor.respond_to?(:path) && actor.last_position == [x, y, z])
                $logger.info "actor #{actor} falling to [#{x}, #{y}, #{fall_to_z}]"
                actor.set_position(x, y, fall_to_z)
                actor.calculate_path(true) if actor.respond_to?(:path)
            end
        end
    end

    # Eventually this will be handled by managers.
    # Hacky as heck. Find an actor with a task with a job, and reference the
    # jobmanager there. Will obviously fail if no workers are currently assigned
    # tasks from the jobmanager.
    # FIXME: Where to get jobmanager ref?
    # FIXME: Currently sledgehammers ALL blocked paths.
    def invalidate_blocked_paths
        actor = @actors.find { |a| a.respond_to?(:task) && a.task && a.task.job }
        actor.task.job.jobmanager.invalidate_blocked_paths if actor
    end

    def get_tile_type(x, y, z)
        self.terrain.get_tile_type(x, y, z)
    end

    def get_surface_level(x, y); self.terrain.get_surface_level(x, y); end
    def out_of_bounds?(x,y,z); self.terrain.out_of_bounds?(x,y,z); end
    def on_edge?(x,y,z); (x == 0 || y == 0 || x == (self.width-1) || y == (self.height-1)); end

    # Calculate where actors above the tile would fall.
    def fall_to_z(x, y, fall_to_z)
        while fall_to_z > 0 && !solid_ground?(x, y, fall_to_z)
            fall_to_z -= 1
        end
        # We've reached a non-empty tile, so go back up to the last empty tile.
        fall_to_z += 1
    end

    def solid_ground?(x, y, z)
        return false if z < 0
        tile = self.terrain.get_tile_type(x, y, z)
        !(tile.nil? || tile.ancestors.include?(LiquidModule))
    end

    def all_in_chebyshev_distance(position, distance)
        range = (-distance..distance).to_a
        d1 = distance * 2 + 1
        d2 = d1 * d1
        d3 = d2 * d1

        Array.new(d3) { position.dup }.each_with_index do |p, i|
            num, remainder = i.divmod(d2)
            p[0] += range[num]
            num, remainder = remainder.divmod(d1)
            p[1] += range[num]
            p[2] += range[remainder]
        end
    end

    # Not actually 'pathable to from position,' but just 'standable.'
    def pathable_in_distance(position, distance)
        all_in_chebyshev_distance(position, distance).select { |p| pathable?(*p) }
    end

    def pathable?(x, y, z)
        !out_of_bounds?(x, y, z) &&
        get_tile_type(x, y, z).nil? &&
        solid_ground?(x, y, z - 1)
    end

    def update(elapsed)
        active_camera.update if active_camera.respond_to?(:update)

        @actors.each do |actor|
            actor.update(elapsed) if actor.respond_to?(:update)
            actor.nutrition -= 1 if actor.respond_to?(:nutrition)
        end

        self.liquid_system.update if liquid_enabled
    end

    # The World is in charge of creating Actors.
    def create(klass, name, model=nil, material=nil)
        $logger.info("Creating a[n] #{klass}")
        if model.nil?
            model = klass.respond_to?(:model) ? klass.model : klass.to_s
        end
#        if material.nil?
#            material = klass.respond_to?(:material) ? klass.material : klass.to_s
#        end

        actor = nil
        if klass.ancestors.include?(Actor)
            actor = create_actor(klass, name, model, material)
            actor.name = name
            actor.world = self
        elsif klass.ancestors.include?(MHEntity)
            actor = create_entity(klass, name, model, material)
        else
            raise RuntimeError, "Not an Actor or MHEntity class: #{klass}"
        end

        @actors << actor
        actor
    end

    def find(type, criteria={})
        return nil if type.nil?
        found = @actors.select do |a|
            !@locked_actors.include?(a) && a.class.ancestors.include?(type)
        end
        criteria.each_pair do |k,v|
            found.reject! do |a|
#                $logger.info "actor #{a} #{!a.respond_to?(k)} || #{a.send(k) != v}"
                !a.respond_to?(k) || a.send(k) != v
            end
        end
        $logger.info "Found #{found.empty? ? "no" : found.size.to_s} #{type.to_s.humanize} #{criteria != {} ? "with criteria #{criteria}" : ""} : #{found}"
        found
    end

    def lock(object)
        if @actors.include?(object)
            @locked_actors << object
        end
    end

    def unlock(object)
        @locked_actors.delete(object)
    end

    def destroy(thing)
        if thing.is_a?(Actor)
            destroy_actor(thing.name)
        elsif thing.is_a?(MHEntity)
            # How to destroy MHEntities? We'll need to have stored the name *somewhere.*
            name = nil# ???
            destroy_entity(name)
        else
            raise RuntimeError, "Not an Actor or MHEntity class: #{thing}"
        end
        @actors.delete(thing)
        @locked_actors.delete(thing)
    end
end
