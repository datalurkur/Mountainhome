require 'Terrain'
require 'TerrainBuilder'

class TerrainVerificationDecorator
    def initialize(terrain)
        @array = Array.new(terrain.width) { Array.new(terrain.height) { Array.new(terrain.depth) { nil } } }
        @terrain = terrain
    end

    def verify
        (@terrain.height - 1).downto(0) do |y|
            line = []
            (@terrain.width - 1).downto(0) do |x|
                z = @terrain.get_surface(x, y)
                line << "#{z} [#{get_backup_surface(x, y)}]"
            end
            $logger.info line.join(" ")
        end

        (@terrain.height - 1).downto(0) do |y|
            line = []
            (@terrain.width - 1).downto(0) do |x|
                z = @terrain.get_surface(x, y)
                line << "%4s " % [(z - get_backup_surface(x, y)).to_s]
            end
            $logger.info line.join(" ")
        end
    end

    def get_backup_surface(x, y)
        zLevel = -1
        @array[x][y].each_with_index do |type, index|
            zLevel = index if type
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
    def initialize(width, height, depth, core)
        super(width, height, depth, core)

        # Setup the camera
        camera.set_fixed_yaw(0, 0, 1)
        camera.set_position(0.25 * width, 0.25 * height, 3 * depth)
        camera.look_at(0.55 * width, 0.45 * height, 0)

        # And define some initial values.
        @pitch = 0
        @yaw = 0
        @movement = [0, 0, 0]

        # Generate a predictable world to see the effects of turning various terrainbuilder features on and off
        seed = rand(100000)
        # seed = 48103
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

        # Do the actual world generation and benchmark it as we go.
        start_bench()
        mark_time("add_layer"      ); TerrainBuilder.add_layer(terrain, 1, 0.0, 1.0, 5000.0, 0.55)
        mark_time("composite_layer"); TerrainBuilder.composite_layer(terrain, 2, 0.2, 0.4, 5000.0, 0.3)
        mark_time("shear"          ); TerrainBuilder.shear(terrain, 10, true, 1)
        mark_time("shear"          ); TerrainBuilder.shear(terrain,  5, true, 1, 1)
        mark_time("average"        ); TerrainBuilder.average(terrain, 2)
        stop_bench()

        # Print the verification information if it is available.
        terrain.verify if terrain.respond_to?(:verify)

        self.populate
    end

    def start_bench
        @times = Array.new
    end

    def mark_time(name)
        time = Time.now
        $logger.info("Marking time for '#{name}': #{time}")
        @times << [name, time]
    end

    def stop_bench
        stop = Time.now
        details = Array.new
        @times.each_index do |i|
            elapsed = @times[i + 1].nil? ? stop - @times[i][1] : @times[i + 1][1] - @times[i][1]
            details << [@times[i][0], elapsed]
        end
        details << ["total", stop - @times[0][1]]

        width = details.transpose[0].collect(&:size).max
        $logger.info "\n" + (details.collect do |name, elapsed|
            "%-#{width + 1}s #{elapsed}" % [name + ":"]
        end).join("\n")
    end

    def update(elapsed)
        sensitivity = 1.0
        camera.adjust_pitch(@pitch * sensitivity) if @pitch != 0.0
        camera.adjust_yaw(  @yaw   * sensitivity) if @yaw   != 0.0
        @pitch = @yaw = 0

        move = @movement.collect {|elem| elem * elapsed}
        camera.move_relative(*move)
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
            when Keyboard.KEY_UP
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
            when Keyboard.KEY_DOWN
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
            when Keyboard.KEY_LEFT
                if params[:state] == :pressed or params[:state] == :typed
                    @movement[0] = -movement_speed
                else
                    @movement[0] = 0 if @movement[0] < 0
                end
                return :handled
            when Keyboard.KEY_RIGHT
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
