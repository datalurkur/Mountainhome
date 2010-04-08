require 'Terrain'
require 'TerrainBuilder'

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
        $logger.info "Building terrain with seed #{seed}"
        srand(seed)

        # Create the terrain object
        TerrainBuilder.add_layer(terrain,       1, 0.0, 1.0, 5000.0, 0.55)
        TerrainBuilder.average(terrain, 1)
        TerrainBuilder.composite_layer(terrain, 2, 0.2, 0.4, 5000.0, 0.3)
        TerrainBuilder.shear(terrain, 10)
        TerrainBuilder.shear(terrain,  5)
        TerrainBuilder.average(terrain, 1)

        self.populate
    end

    def update(elapsed)
        sens_cap = 0.5
        pitch = [[@pitch * elapsed, sens_cap].min, -sens_cap].max
        yaw =   [[@yaw * elapsed,   sens_cap].min, -sens_cap].max
        camera.adjust_pitch(pitch) if pitch != 0.0
        camera.adjust_yaw(yaw) if yaw != 0.0
        @pitch = @yaw = 0

        move = @movement.collect {|elem| elem * elapsed}
        camera.move_relative(*move)
    end

    def input_event(params={})
        case params[:type]
        when :move
            rotate_speed = -0.001
            @yaw = params[:x] * rotate_speed
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
