require 'Camera'
require 'TerrainBuilder'
require 'Reticle'

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

class MHTerrain
    include TileProperty

    def lookup
        @@lookup
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
                z = @world.get_surface(x, y)
                line << "%2s [%-2s]" % [z, get_backup_surface(x, y)]
            end
            $logger.info line.join(" ")
        end

        (@world.height - 1).downto(0) do |y|
            line = []
            0.upto(@world.width - 1) do |x|
                z = @world.get_surface(x, y)
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

    def set_tile_material(x, y, z, type)
        @world.set_tile_material(x, y, z, type)
        @array[x][y][z] = type
    end

    def method_missing(name, *args)
        @terrain.send(name, *args)
    end
end

class World < MHWorld
    attr_reader :builder_fiber
    attr_accessor :actors, :cameras

    def initialize(core, tile_types, action = :load, args={})
        super(core)

        @actors = Array.new
        case action
        when :empty
            if true
                width  = 3
                height = 3
                depth  = 2

                self.load_empty(width, height, depth, core)
                register_tile_types(tile_types)

                0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_material(x, y, 0, 0) } }

                set_tile_material(0, 0, 1, 1)
                set_tile_material(0, 1, 1, 1)
                set_tile_material(0, 2, 1, 1)

                set_tile_material(1, 2, 1, 1)
            else
                width  = 6
                height = 4
                depth  = 2

                self.load_empty(width, height, depth, core)
                register_tile_types(tile_types)
                0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_material(x, y, 0, 1) } }
                0.upto(width - 1) { |x| 0.upto(height - 1) { |y| set_tile_material(x, y, 1, 1) } }
                set_tile_empty(3, 3, 1)
                set_tile_empty(3, 2, 1)
            end

            self.terrain.poly_reduction = true
            self.terrain.auto_update = true

            self.initialize_pathfinding

            @builder_fiber = Fiber.new { true }
        when :generate
            width  = args[:width]  || 129
            height = args[:height] || 129
            depth  = args[:depth]  || 65

            self.load_empty(width, height, depth, core)
            register_tile_types(tile_types)

            # Generate a predictable world to see the effects of turning various terrainbuilder features on and off
            seed = rand(100000)
            seed = 39611

            # seed = 33843
            # seed = 99632 # Long poly reduction times for larger sizes.
            # seed = 67717 # SLOW
            # seed = 14012 # A neat world.
            # seed = 48103 # Used for benchmarking

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
                do_builder_step(:add_layer,          nil,  self, Gravel, 0.0, 1.0, 5000.0, 0.55)
                do_builder_step(:composite_layer,    nil,  self, Grass,  0.2, 0.4, 5000.0, 0.3 )

                $logger.info "Creating #{terrain_power/2} seismic shears."
                (terrain_power / 2).to_i.times do
                    do_builder_step(:shear,          nil,  self, (rand(terrain_power)+1), 1, 1)
                end

                $logger.info "Carving #{terrain_power} tunnels."
                terrain_power.times do
                    do_builder_step(:form_tunnel,    nil,  self)
                end

                #do_builder_step(:generate_riverbeds, nil,  self, 1)
                do_builder_step(:average,            true, self, 1)
                #do_builder_step(:fill_ocean,         true, self)

                $logger.info "Initializing pathfinding."
                @timer.start("Pathfinding Init")
                self.initialize_pathfinding
                @timer.stop

                @timer.to_s.split(/\n/).each { |line| $logger.info line }

                terrain.verify if terrain.respond_to?(:verify)

                $logger.info "World generation finished at #{Time.new}."
                $logger.unindent

                true # To indicate we're done.
            end
        when :load
            # TODO: The load has to be called outside of the fiber for now because the
            # dimensions are set based on the loaded data and the dimensions are ALSO
            # needed for setting up the camera. This means the information needs to be
            # available BEFORE the fiber executes. This is bad, though, because it means
            # we have dead time on the loading screen. This needs to be fixed so we have
            # an immediate jump to the loading screen without any camera exceptions.
            self.load(args[:filename]);
            self.terrain.poly_reduction = true
            self.terrain.auto_update    = true
            self.initialize_pathfinding
            @builder_fiber = Fiber.new { true }
        end

        # Setup the cameras
        @cameras = []
        @cameras << create_camera("TopDownCamera", TopCamera)
        @cameras << create_camera("BasicCamera", BasicCamera)
        @cameras << create_camera("FirstPersonCamera", FirstPersonCamera, nil)

        self.active_camera = @cameras[0]

        @mouselook = false

        # And define some initial values.
        @pitch = 0
        @yaw = 0
        @movement = [0, 0, 0]

        $logger.info("World size: #{width}x#{height}x#{depth}")
    end

    def register_tile_types(types)
        types.each do |klass|
            material_id = terrain.register_tile_material(klass.new.material)
            klass.class_attributes[:material_id] = material_id
        end
    end

    def do_builder_step(name, final, *args)
        # This should work, but poly reduction is actually broken. Leaving this
        # here as a reminder.
        self.terrain.poly_reduction = final
        # self.terrain.poly_reduction = false
        self.terrain.auto_update    = false

        @timer.start(name.to_s)
        TerrainBuilder.send(name, *args)
        @timer.stop

        $logger.info("Step finished. Generating geometry.")
        @timer.start("Populate")
        self.populate()
        @timer.stop

        self.terrain.auto_update = final

        Fiber.yield
    end

    def pathfinding_initialized?; @pathfinding_initialized ||= false; end

    def initialize_pathfinding
        (0...self.width).each do |x|
            (0...self.height).each do |y|
                self.terrain.each_filled_range(x, y) do |start_z, end_z|
                    self.pathfinder.block_z_range(x, y, start_z, end_z)
                end
            end
        end
        pathfinding_initialized = true
    end

    def set_tile_material(x, y, z, type)
        if type == self.terrain.lookup[:Empty]
            self.set_tile_empty(x, y, z)
        else
            self.terrain.set_tile_numeric_property(x, y, z, self.terrain.lookup[:Material], type)
            self.pathfinder.block_tile(x, y, z) if self.pathfinding_initialized?
        end
    end

    def set_tile_empty(x, y, z)
        self.terrain.set_tile_empty(x, y, z)
        self.pathfinder.unblock_tile(x, y, z) if self.pathfinding_initialized?
    end

    def get_tile_material(x, y, z)
        self.terrain.get_tile_numeric_property(x, y, z, self.terrain.lookup[:Material])
    end

    def get_surface(x, y); self.terrain.get_surface(x, y); end
    def out_of_bounds?(x,y,z); self.terrain.out_of_bounds?(x,y,z); end

    def update(elapsed)
        sensitivity = 1.0
        self.active_camera.adjust_pitch(@pitch * sensitivity) if (@pitch != 0.0 && self.active_camera.respond_to?(:adjust_pitch))
        self.active_camera.adjust_yaw(  @yaw   * sensitivity) if (@yaw   != 0.0 && self.active_camera.respond_to?(:adjust_yaw))
        @pitch = @yaw = 0

        move = @movement.collect {|elem| elem * elapsed}
        self.active_camera.move_relative(*move) if self.active_camera.respond_to?(:move_relative) && move != [0.0, 0.0, 0.0]

        self.active_camera.update if self.active_camera.respond_to?(:update)

        # update actors
        @actors.each { |actor|
            actor.update(elapsed) if actor.respond_to?(:update)
        }
    end

    def toggle_mouselook
        @mouselook = !@mouselook
    end

    def input_event(event)
        case event
        when MouseMoved
            if @mouselook
                rotate_speed = -0.002
                @yaw   = event.relX * rotate_speed
                @pitch = event.relY * rotate_speed
                return :handled
            end
        when KeyPressed, KeyReleased
            movement_speed = 0.01
            case event.key
            when Keyboard.KEY_UP, Keyboard.KEY_w
                if event.state == :pressed
                    if event.shift_held?
                        @movement[2] = -movement_speed 
                    else
                        @movement[1] = movement_speed
                    end
                else
                    @movement[1] = 0 if @movement[1] > 0
                    @movement[2] = 0 if @movement[2] < 0
                end
                return :handled
            when Keyboard.KEY_DOWN, Keyboard.KEY_s
                if event.state == :pressed
                    if event.shift_held?
                        @movement[2] = movement_speed
                    else
                        @movement[1] = -movement_speed
                    end
                else
                    @movement[1] = 0 if @movement[1] < 0
                    @movement[2] = 0 if @movement[2] > 0
                end
                return :handled
            when Keyboard.KEY_LEFT, Keyboard.KEY_a
                if event.state == :pressed
                    @movement[0] = -movement_speed
                else
                    @movement[0] = 0 if @movement[0] < 0
                end
                return :handled
            when Keyboard.KEY_RIGHT, Keyboard.KEY_d
                if event.state == :pressed
                    @movement[0] = movement_speed
                else
                    @movement[0] = 0 if @movement[0] > 0
                end
                return :handled
            end
        end
        return :unhandled
    end

    # The World is in charge of creating Actors.
    def create(klass, name, model=klass.to_s, material=klass.to_s)
        $logger.info("Creating a[n] #{klass}")

        actor = nil
        if klass.ancestors.include?(Actor)
            actor = create_actor(klass, name, model, material)
            actor.name = name
        elsif klass.ancestors.include?(MHEntity)
            actor = create_entity(klass, name, model, material)
        else
            raise RuntimeError, "Not an Actor class: #{klass}"
        end

        @actors << actor
        actor
    end
end
