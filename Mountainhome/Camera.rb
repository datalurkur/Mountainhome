class Camera
    attr_accessor :camera, :name
    def initialize(name, world)
        @name  = name
        @world = world

        if @world.nil?
            $logger.error "Camera initialized without a world object!"
        else
            @camera = @world.create_camera(@name)
        end
    end

    def set_active
        if @camera.nil?
            $logger.error "Cannot set uninitialized camera to active"
        else
            @world.active_camera = self
        end
    end
end

class TopCamera < Camera
    def initialize(name, world)
        super(name, world)

        if @camera.nil?
            $logger.error "TopCamera failed to initialize"
        else
            @camera.center_ortho(@world.width * 2, @world.width * 0.5, @world.height * 0.5, -@world.depth, 0.0)
        end
    end
end

class IsoCamera < Camera
    def initialize(name, world)
        super(name, world)
        
        if @camera.nil?
            $logger.error "IsoCamera failed to initialize"
        else
            @camera.set_fixed_yaw(0, 0, 1)
            @camera.set_position(0.25 * @world.width, 0.25 * @world.height, (@world.width + @world.height) * 0.5 + (@world.depth) * 0.5)
            @camera.look_at(0.55 * @world.width, 0.45 * @world.height, 0)
        end
    end
end
