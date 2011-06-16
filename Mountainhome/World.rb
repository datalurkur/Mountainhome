require 'TerrainBuilder'
require 'Reticle'

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
        @uninitialized_liquid = []

        case action
        when :empty
            if false
                width  = 1
                height = 1
                depth  = 1

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new {
                    self.initialize_liquid
                    self.initialize_pathfinding
                }
            elsif true
                width  = 3
                height = 3
                depth  = 3

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new do
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_type(x, y, 0, Dirt) } }

                    set_tile_type(0, 0, 0, Rock)
                    set_tile_type(0, 1, 1, Rock)
                    set_tile_type(0, 2, 1, Rock)
                    set_tile_type(1, 0, 1, Water)
                    set_tile_type(1, 1, 1, Water)
                    set_tile_type(1, 2, 1, Rock)

                    self.initialize_liquid
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

                    self.initialize_liquid
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
            seed = rand(100000)

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

                # Args: world, ???, ???, ???
                prepare_builder_step(:form_strata, self, [Rock, Dirt], 0, terrain_power*10)

                $logger.info "Carving #{terrain_power} tunnels."
                terrain_power.times do
                    # Args: world
                    prepare_builder_step(:form_tunnels, self)
                end

                # Args: world, ???
                prepare_builder_step(:average, self, 1)

                # Args: world, ???
                prepare_builder_step(:generate_riverbeds, self, 2)

                # Args: world, what to fill the ocean with.
                prepare_builder_step(:fill_ocean, self, Water)

                # Args: world, chance of dirt growing grass.
                prepare_builder_step(:handle_tile_growth, self, 1)

                prepare_builder_step(:final)

                $logger.info "Initializing liquids"
                @timer.start("Liquid Initialization")
                self.initialize_liquid
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
            self.initialize_liquid
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

    def pathfinding_initialized?; @pathfinding_initialized ||= false; end
    def initialize_pathfinding
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

    def liquid_initialized?; @liquid_initialized ||= false; end
    def initialize_liquid
        # AJEAN - SET UP LIQUID STATE
        @outflows ||= {}
        @inflows  ||= {}
        @uninitialized_liquid.each do |coords|
            type = self.get_tile_type(*coords)
            process_liquid(coords, -rand(type.flow_rate))
        end
        @uninitialized_liquid = []
        @liquid_initialized = true
    end

    def local_neighbors(coords)
        n = [[-1,  1], [ 0,  1], [ 1,  1],
         [-1,  0],           [ 1,  0],
         [-1, -1], [ 0, -1], [ 1, -1]].collect do |local|
            [coords.x + local.x, coords.y + local.y, coords.z]
        end
        n.reject { |i| out_of_bounds?(*i) }
    end

    def set_flow(source, dest, time)
        #$logger.info "Water set to flow from #{source} to #{dest} in #{time}"
        @outflows[source] = [dest, time]
        @inflows[dest]    = true
    end

    def do_flows(elapsed)
        flowed = []
        t_start = Time.now
        @outflows.keys.each do |source|
            info = @outflows[source]
            info[1] -= elapsed
            if info[1] <= 0
                #$logger.info "[+] Flowing into #{info[0]} (from #{source})"
                # Queue up the source to be emptied
                flowed << source

                # Fill the destination and clear it from the inflows hash
                outflow_type = self.get_tile_type(*source)
                self.set_tile_type(*(info[0]), outflow_type, info[1])
                @inflows.delete(info[0])
            end
        end

        flowed.each do |outflow|
            info = @outflows[outflow]
            #$logger.info "[-] Flowing out of #{outflow} (into #{info[0]})"

            # Empty the source and clear it from the outflows hash
            self.set_tile_type(*(outflow), nil, info[1])
            @outflows.delete(outflow)
        end
        t_end = Time.now
        tick_time = t_end - t_start

        $logger.info "#{flowed.size} nodes flowed in #{tick_time}" unless flowed.size == 0
    end

    def process_vacuum(coords, offset)
        unless @inflows[coords].nil?
            $logger.warn "Vacuum at #{coords} already has a scheduled inflow (THIS PROBABLY SHOULDN'T HAPPEN)"
            return
        end

        # Check tile above
        above = [coords.x, coords.y, coords.z + 1]
        unless out_of_bounds?(*above)
            type = self.get_tile_type(*above)
            if type && type.ancestors.include?(LiquidModule) && @outflows[above].nil?
                set_flow(above, coords, type.flow_rate + offset)
                return
            end
        end

        # Check neighboring tiles
        eligibles = []
        local_neighbors(coords).each do |neighbor|
            type = self.get_tile_type(*neighbor)
            if type && type.ancestors.include?(LiquidModule) && @outflows[neighbor].nil?
                eligibles << [neighbor, type]
            end
        end
        unless eligibles.empty?
            random_eligible = eligibles[rand(eligibles.size)]
            set_flow(random_eligible[0], coords, random_eligible[1].flow_rate + offset)
            return
        end
    end

    def process_liquid(coords, offset)
        unless @outflows[coords].nil?
            $logger.warn "Liquid at #{coords} is already scheduled to flow (THIS PROBABLY SHOULDN'T HAPPEN)"
            return
        end
        source_type = self.get_tile_type(*coords)

        # Check tile below
        below = [coords.x, coords.y, coords.z - 1]
        unless out_of_bounds?(*below)
            if self.get_tile_type(*below).nil? && @inflows[below].nil?
                set_flow(coords, below, source_type.flow_rate + offset)
                return
            end
        end

        # Check neighboring tiles
        eligibles = []
        local_neighbors(coords).each do |neighbor|
            if self.get_tile_type(*neighbor).nil? && @inflows[neighbor].nil?
                eligibles << neighbor
            end
        end
        unless eligibles.empty?
            set_flow(coords, eligibles[rand(eligibles.size)], source_type.flow_rate + offset)
            return
        end
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

    def set_tile_type(x, y, z, tile, timer_offset = 0)
        # FIXME This will just slow things down. Consider moving to C where we can wrap
        # the protection in DEBUG build only macros.
        if !tile.nil? && !tile.kind_of?(Class)
            raise RuntimeError, "Do not use set_tile_type with instances of tile types."
        end

        self.terrain.set_tile_type(x, y, z, tile)

        # AJEAN - UPDATE LIQUID STATE
        if liquid_initialized?
            if tile.nil?
                process_vacuum([x,y,z], timer_offset)
            elsif tile.ancestors.include?(LiquidModule)
                process_liquid([x,y,z], timer_offset)
            end
        elsif tile && tile.ancestors.include?(LiquidModule)
            @uninitialized_liquid << [x,y,z]
        end

        if pathfinding_initialized?
            if tile
                self.pathfinder.set_tile_closed(x, y, z)
                if z + 1 < self.depth && !self.get_tile_type(x, y, z + 1).nil?
                    if tile.ancestors.include?(LiquidModule)
                        self.pathfinder.set_tile_open(x, y, z + 1)
                    else
                        self.pathfinder.set_tile_pathable(x, y, z + 1)
                    end
                end
            else
                if solid_ground?(x, y, z - 1)
                    self.pathfinder.set_tile_pathable(x, y, z)
                else
                    self.pathfinder.set_tile_open(x, y, z)
                end

                if z + 1 < self.depth && self.get_tile_type(x, y, z + 1).nil?
                    self.pathfinder.set_tile_open(x, y, z + 1)
                end
            end
        end

        # TODO - Register tile events with an event handler system so that we can move all of this code to more appropriate places
        # Here's where we handle things that need to happen for mining/tile removal.
        # This is not intended to live here permanently, and much of this code is
        # terribly inefficient.
        if tile.nil?
            # Empty tiles shouldn't be selected.
            deselect_tile(x, y, z)
            # Calculate where actors above the tile would fall.
            fall_to_z = z
            while fall_to_z >= 0 && self.terrain.get_tile_type(x,y,fall_to_z).nil?
                fall_to_z -= 1
            end
            # We've reached a non-empty tile, so go back up to the last empty tile.
            fall_to_z += 1
            @actors.each do |actor|
                # Check for actors above the tile, and make them 'fall.'
                if actor.position == [x, y, z + 1]
                    $logger.info "actor #{actor} falling to [#{x}, #{y}, #{fall_to_z}]"
                    actor.set_position(x, y, fall_to_z)
                end
                # If the actor was going to move over the removed tile, invalidate the
                # task and job. Eventually this will be handled by managers.
                if actor.respond_to?(:path) && actor.path && actor.path.include?([x, y, z + 1])
                    if actor.respond_to?(:task) && actor.task && actor.task.job
                        # Sledgehammer all blocked paths.
                        actor.task.job.jobmanager.invalidate_blocked_paths
                    end
                    # Handle fractional-position interruption.
                    # FIXME: These position assignments can make the dwarf teleport
                    # if traversing a large node. Really we should reset it to the
                    # *nearest traversible* tile.
                    if actor.position != actor.position.collect { |i| i.to_i }
                        # Was xyz+1 being pathed to at that moment?
                        if actor.path.first == [x, y, z + 1]
                            actor.path.shift
                            actor.position = [x, y, z]
                        else # [x, y, z + 1] was reached later.
                            actor.position = actor.path.shift
                        end
                    end
                    actor.calculate_path
                end
            end
        end
    end

    def get_tile_type(x, y, z)
        self.terrain.get_tile_type(x, y, z)
    end

    def get_surface_level(x, y); self.terrain.get_surface_level(x, y); end
    def out_of_bounds?(x,y,z); self.terrain.out_of_bounds?(x,y,z); end

    def solid_ground?(x,y,z)
        return false if z < 0

        tile = self.terrain.get_tile_type(x,y,z)
        !(tile.nil? || tile.ancestors.include?(LiquidModule))
    end

    def update(elapsed)
        active_camera.update if active_camera.respond_to?(:update)

        @actors.each do |actor|
            actor.update(elapsed) if actor.respond_to?(:update)
        end

        # AJEAN - UPDATE LIQUID STATE
        do_flows(elapsed)
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
    end
end
