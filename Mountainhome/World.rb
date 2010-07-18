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

    def to_s
        @times << ["total", @times.transpose[1].inject(0, &:+)]
        width = @times.transpose[0].collect(&:size).max + 1
        "\n" + @times.collect { |name, elapsed| "%-#{width}s #{elapsed}" % ["#{name}:"] }.join("\n")
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

        # $logger.info "Printing cross sections"
        # 0.upto(@terrain.height - 1) do |y|
        #     $logger.info "Printing cross section of height #{y}"
        #     (@terrain.depth - 1).downto(0) do |z|
        #         line = Array.new
        #         0.upto(@terrain.width - 1) do |x|
        #             line << "%2s [%2s,%2s,%2s]" % [@terrain.get_tile(x, y, z), x, y, z]
        #         end
        #         $logger.info line.join(" ")
        #     end
        # end
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
    attr_accessor :actor_list
    
    def initialize(core, action = :load, args={})
        super(core)

        case action
        when :empty
            width  = args[:width]  || 8
            height = args[:height] || 8
            depth  = args[:depth]  || 8

            self.load_empty(width, height, depth, core)
            0.upto(width - 1) { |x| 0.upto(height - 1) { |y| terrain.set_tile(x, y, 0, 1) } }
            @builder_fiber = Fiber.new { true }
        when :generate
            width  = args[:width]  || 129
            height = args[:height] || 129
            depth  = args[:depth]  || 65

            self.load_empty(width, height, depth, core)

            # Generate a predictable world to see the effects of turning various terrainbuilder features on and off
            seed = rand(100000)
            # seed = 99632 # Long poly reduction times for larger sizes.
            # seed = 67717 # SLOW
            # seed = 14012 # A neat world.
            # seed = 48103 # Used for benchmarking

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
                do_builder_step(:shear,              nil,  terrain, 10, 1, 1)
                do_builder_step(:shear,              nil,  terrain, 5,  1, 1)
                do_builder_step(:generate_riverbeds, nil,  terrain, 1)
                do_builder_step(:average,            nil,  terrain, 2)
                do_builder_step(:fill_ocean,         true, terrain, liquid_manager)
                @timer.to_s.split(/\n/).each { |line| $logger.info line }

                terrain.verify if terrain.respond_to?(:verify)

                $logger.info "World generation finished."
                $logger.unindent

                true # To indicate we're done.
            end
        when :load
            self.load(args[:filename])
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

        @actor_list = []
        $logger.info("Time to create a dwarf!")
        create_actor(Dwarf, "Franzibald", "Sphere", [0, 0, self.terrain.get_surface(0,0)+1])
    end

    def do_builder_step(name, final, *args)
        # This should work, but poly reduction is actually broken. Leaving this here as a
        # reminder.
        # self.terrain.poly_reduction = final

        self.terrain.poly_reduction = false
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

    def update(elapsed)
        sensitivity = 1.0
        self.camera.adjust_pitch(@pitch * sensitivity) if @pitch != 0.0
        self.camera.adjust_yaw(  @yaw   * sensitivity) if @yaw   != 0.0
        @pitch = @yaw = 0

        move = @movement.collect {|elem| elem * elapsed}
        self.camera.move_relative(*move)

        # update actors
        @actor_list.each { |actor|
            actor.update(elapsed)
        }
    end

    def input_event(event)
        case event[:type]
        when :move
            rotate_speed = -0.002
            @yaw   = event[:x] * rotate_speed
            @pitch = event[:y] * rotate_speed
            return :handled
        when :keyboard
            movement_speed = 0.05
            case event[:key]
            when Keyboard.KEY_UP, Keyboard.KEY_w
                if event[:state] == :pressed or event[:state] == :typed
                    if event[:modifier] == 1
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
                if event[:state] == :pressed or event[:state] == :typed
                    if event[:modifier] == 1
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
                if event[:state] == :pressed or event[:state] == :typed
                    @movement[0] = -movement_speed
                else
                    @movement[0] = 0 if @movement[0] < 0
                end
                return :handled
            when Keyboard.KEY_RIGHT, Keyboard.KEY_d
                if event[:state] == :pressed or event[:state] == :typed
                    @movement[0] = movement_speed
                else
                    @movement[0] = 0 if @movement[0] > 0
                end
                return :handled
            end
        end
        return :unhandled
    end
    
    def find_path(sX, sY, dX, dY)
        timer = Timer.new
        timer.reset
        timer.start("Pathfinding")
        path = super(sX, sY, dX, dY)
        timer.stop
        $logger.info timer.to_s

        path
    end

    # The World is in charge of creating Actors.
    def create_actor(klass, name, model, pos)
      # Only actors can be created with Entities currently.
      
      actor = klass.new
      unless actor.is_a?(Actor)
        $logger.error("Not an Actor class: #{klass}")
        return
      end

      actor.name = name
      actor.position = pos
      actor.world = self

      $logger.info("Creating actor #{name}")
      
      # When an Actor is created, a corresponding Entity is created in C and associated with the Actor.
      actor.entity = create_entity(name, model, pos[0], pos[1], pos[2])
      
      # actors are tracked in Ruby by World
      @actor_list << actor
    end

    # Will need to dereference the Actor and delete Entity associated.
    def delete_actor(actor)
      original_size = @actor_list.size
      @actor_list.delete(actor)
      unless @actor_list.size == original_size - 1
        $logger.error("Error deleting actor #{actor}")
        return
      end
      # Entity deletion needs to happen in C++.
      delete_entity(actor.name)
    end
end
