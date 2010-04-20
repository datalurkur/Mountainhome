require 'Terrain'
require 'TerrainBuilder'

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

    def print_stats
        @times << ["total", @times.transpose[1].inject(0, &:+)]
        width = @times.transpose[0].collect(&:size).max
        $logger.info "\n" + (
            @times.collect do |name, elapsed|
                "%-#{width + 1}s #{elapsed}" % [name + ":"]
            end).join("\n")
    end
end

class TerrainVerificationDecorator
    def initialize(terrain)
        @array = Array.new(terrain.width) { Array.new(terrain.height) { Array.new(terrain.depth) { nil } } }
        @terrain = terrain
    end

    def verify
        $logger.info "Verifying world!"
        (@terrain.height - 1).downto(0) do |y|
            line = []
            (@terrain.width - 1).downto(0) do |x|
                z = @terrain.get_surface(x, y)
                line << "%2s [%-2s]" % [z, get_backup_surface(x, y)]
            end
            $logger.info line.join(" ")
        end

        (@terrain.height - 1).downto(0) do |y|
            line = []
            0.upto(@terrain.width - 1) do |x|
                z = @terrain.get_surface(x, y)
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



    def set_tile(x, y, z, type)
        @terrain.set_tile(x, y, z, type)
        @array[x][y][z] = type
    end



    def method_missing(name, *args)
        @terrain.send(name, *args)
    end
    
end

class World < MHWorld
    attr_reader :builder_fiber
    def initialize(core, action = :load, args={})
        super(core)

        case action
        when :generate
            width  = args[:width]  || 129
            height = args[:height] || 129
            depth  = args[:depth]  || 65

            self.load_empty(width, height, depth, core)

        # Generate a predictable world to see the effects of turning various terrainbuilder features on and off
        seed = rand(100000)
        # seed = 74984
        seed = 67717 # Borken!!
        # seed = 14012 # A neat world.
        # seed = 48103 # Used for benchmarking
        # seed = 15630 # Broken @ 257, 257, 65! Looks like it was attacked by the M$ pipes screen saver.
        $logger.info "Building terrain with seed #{seed}"
        srand(seed)

            # Verify the integrity of the octree
            #TerrainBuilder.test_populate(terrain)
            #terrain.clear

            # Get the terrain object and install a special decorator to verify our results
            # if the map is small enough to make it feasible.
            terrain = self.terrain
            if terrain.width < 32 && terrain.height < 32 && terrain.depth < 32
                terrain = TerrainVerificationDecorator.new(self.terrain)
            end

            @timer = Timer.new
            @builder_fiber = Fiber.new do
                # Do the actual world generation and benchmark it as we go.
                $logger.info "Starting world generation:"
                $logger.indent

            @timer.reset
            do_builder_step(:add_layer,          nil,  terrain, 1, 0.0, 1.0, 5000.0, 0.55)
            do_builder_step(:composite_layer,    nil,  terrain, 2, 0.2, 0.4, 5000.0, 0.3 )
            #do_builder_step(:shear,              nil,  terrain, 10, 1, 1)
            #do_builder_step(:shear,              nil,  terrain, 5,  1, 1)
            #do_builder_step(:generate_riverbeds, nil,  terrain, 1)
            do_builder_step(:average,            true, terrain, 2)
            @timer.print_stats

                terrain.verify if terrain.respond_to?(:verify)

                $logger.info "World generation finished."
                $logger.unindent

                true # To indicate we're done.
            end
        when :load
            self.load("test")
            @builder_fiber = Fiber.new { true }
        end

        # Setup the camera
        self.camera.set_fixed_yaw(0, 0, 1)
        self.camera.set_position(0.25 * self.width, 0.25 * self.height, (self.width + self.height) * 0.5 + (self.depth) * 0.5)
        self.camera.look_at(0.55 * self.width, 0.45 * self.height, 0)

        # And define some initial values.
        @pitch = 0
        @yaw = 0
        @movement = [0, 0, 0]
    end

    def do_builder_step(name, reduce, *args)
        @timer.start(name.to_s)
        TerrainBuilder.send(name, *args)
        @timer.stop

        $logger.info("Step finished. Generating geometry.")
        self.populate(reduce)
        Fiber.yield
    end

    def update(elapsed)
        sensitivity = 1.0
        self.camera.adjust_pitch(@pitch * sensitivity) if @pitch != 0.0
        self.camera.adjust_yaw(  @yaw   * sensitivity) if @yaw   != 0.0
        @pitch = @yaw = 0

        move = @movement.collect {|elem| elem * elapsed}
        self.camera.move_relative(*move)
    end

    def input_event(params={})
        case params[:type]
        when :move
            rotate_speed = -0.002
            @yaw   = params[:x] * rotate_speed
            @pitch = params[:y] * rotate_speed
            return :handled
        when :keyboard
            movement_speed = 0.05
            case params[:key]
            when Keyboard.KEY_UP, Keyboard.KEY_w
                if params[:state] == :pressed or params[:state] == :typed
                    if params[:modifier] == 1
                        @movement[1] = movement_speed
                    else
                        @movement[2] = -movement_speed 
                    end
                else
                    @movement[1] = 0 if @movement[1] > 0
                    @movement[2] = 0 if @movement[2] < 0
                end
                return :handled
            when Keyboard.KEY_DOWN, Keyboard.KEY_s
                if params[:state] == :pressed or params[:state] == :typed
                    if params[:modifier] == 1
                        @movement[1] = -movement_speed
                    else
                        @movement[2] = movement_speed
                    end
                else
                    @movement[1] = 0 if @movement[1] < 0
                    @movement[2] = 0 if @movement[2] > 0
                end
                return :handled
            when Keyboard.KEY_LEFT, Keyboard.KEY_a
                if params[:state] == :pressed or params[:state] == :typed
                    @movement[0] = -movement_speed
                else
                    @movement[0] = 0 if @movement[0] < 0
                end
                return :handled
            when Keyboard.KEY_RIGHT, Keyboard.KEY_d
                if params[:state] == :pressed or params[:state] == :typed
                    @movement[0] = movement_speed
                else
                    @movement[0] = 0 if @movement[0] > 0
                end
                return :handled
            end
        end
        return :unhandled
    end
end
