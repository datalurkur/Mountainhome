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

    def set_voxel_type(x, y, z, voxel)
        @world.set_voxel_type(x, y, z, voxel)
        @array[x][y][z] = voxel.class
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

        case action
        when :empty
            if false
                width  = 1
                height = 1
                depth  = 1

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new {
                    self.liquid_manager.setup(self.terrain)
                }
            elsif true
                width  = 5
                height = 5
                depth  = 5

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new do
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_voxel_type(x, y, 0, Dirt) } }

                    set_voxel_type(0,0,1,Dirt)
                    set_voxel_type(2,2,1,Dirt)

                    set_voxel_type(0,0,2,Rock)
                    set_voxel_type(0,1,2,Rock)
                    set_voxel_type(1,3,2,Rock)
                    set_voxel_type(1,4,2,Rock)
                    set_voxel_type(2,4,2,Rock)
                    set_voxel_type(3,3,2,Water)

                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_voxel_type(x, y, 3, Water) } }

                    set_voxel_type(4,4,4,Water)

                    self.liquid_manager.setup(self.terrain)
                end
            else
                width  = 6
                height = 4
                depth  = 2

                self.load_empty(width, height, depth, core)

                @builder_fiber = Fiber.new do
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_voxel_type(x, y, 0, Rock) } }
                    0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_voxel_type(x, y, 1, Rock) } }
                    set_voxel_type(3, 3, 1, nil)
                    set_voxel_type(3, 2, 1, nil)

                    self.liquid_manager.setup(self.terrain)
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
            # seed = 98724 # floating voxel bug on 9x9x33
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
                prepare_builder_step(:handle_voxel_growth, self, 1)

                prepare_builder_step(:final)

                $logger.info "Initializing liquids"
                @timer.start("Liquid Initialization")
                self.liquid_manager.setup(self.terrain)
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
            self.liquid_manager.setup(self.terrain)
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

    attr_accessor :liquid_flows_enabled

    def local_neighbors(coords)
        n = [[-1,  1], [ 0,  1], [ 1,  1],
             [-1,  0],           [ 1,  0],
             [-1, -1], [ 0, -1], [ 1, -1]].collect do |local|
            [coords.x + local.x, coords.y + local.y, coords.z]
        end
        n.reject { |i| out_of_bounds?(*i) }
    end

    def select_voxel(x, y, z)
        set_voxel_parameter(x, y, z, :selected, true)
    end

    def deselect_voxel(x, y, z)
        set_voxel_parameter(x, y, z, :selected, false)
    end

    # TODO - Register voxel events with an event handler system so that we can move all of this code to more appropriate places
    def set_voxel_type(x, y, z, voxel, timer_offset = 0)
        # FIXME This will just slow things down. Consider moving to C where we can wrap
        # the protection in DEBUG build only macros.
        if !voxel.nil? && !voxel.kind_of?(Class)
            raise RuntimeError, "Do not use set_voxel_type with instances of voxel types."
        end

        self.terrain.set_voxel_type(x, y, z, voxel)

        if voxel.nil?
            self.liquid_manager.vacuum_created(x, y, z)
        elsif voxel.ancestors.include?(LiquidModule)
            self.liquid_manager.liquid_created(x, y, z)
        end

        # Empty voxels shouldn't be selected.
        deselect_voxel(x, y, z) if voxel.nil?
    end

    def get_voxel_type(x, y, z)
        self.terrain.get_voxel_type(x, y, z)
    end

    def get_surface_level(x, y); self.terrain.get_surface_level(x, y); end
    def out_of_bounds?(x,y,z); self.terrain.out_of_bounds?(x,y,z); end
    def on_edge?(x,y,z); (x == 0 || y == 0 || x == (self.width-1) || y == (self.height-1)); end

    # Calculate where actors above the voxel would fall.
    def fall_to_z(x, y, fall_to_z)
        while fall_to_z > 0 && !solid_ground?(x, y, fall_to_z)
            fall_to_z -= 1
        end
        # We've reached a non-empty voxel, so go back up to the last empty voxel.
        fall_to_z += 1
    end

    def solid_ground?(x, y, z)
        return false if z < 0
        voxel = self.terrain.get_voxel_type(x, y, z)
        !(voxel.nil? || voxel.ancestors.include?(LiquidModule))
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

    def update(elapsed)
        active_camera.update if active_camera.respond_to?(:update)

        @actors.each do |actor|
            actor.update(elapsed) if actor.respond_to?(:update)
            actor.nutrition -= 1 if actor.respond_to?(:nutrition)
        end

        # TODO - Right now this only supports 1 liquid type
        liquid_type = Water
        # In the future, we'll want to get liquid managers by their voxel type (which world will store in, say, a map)
        if self.liquid_manager.update(elapsed)
            self.liquid_manager.each_new_liquid do |liquid|
                # New liquid voxels are created based on their manager's type
                self.set_voxel_type(liquid[0], liquid[1], liquid[2], liquid_type)
            end
            self.liquid_manager.clear_new_liquids
        end
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
