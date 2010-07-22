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

        @z_level    = @world.depth-1
        @center     = [@world.width * 0.5, @world.height * 0.5]
        @zoom_width = @world.width * 2

        if @camera.nil?
            $logger.error "TopCamera failed to initialize"
        else
            recenter
        end
    end

    def recenter
        if self.camera.ratio >= 1
            @camera.center_ortho(@zoom_width*self.camera.ratio, @center[0], @center[1], -@z_level, 0.0)
        else
            @camera.center_ortho(@zoom_width, @center[0], @center[1], -@z_level, 0.0)
        end
    end

    def zoom_height
        @zoom_width / self.camera.ratio
    end

    def change_depth(mod)
        @z_level += mod

        if @z_level < 0
            @z_level = 0
        elsif @z_level > @world.depth
            @z_level = @world.depth
        end

        recenter
    end

    def left_boundary;  @zoom_width/2.0;                     end
    def right_boundary; @world.width - @zoom_width/2.0 - 1;  end
    def lower_boundary; @zoom_width/2.0;                     end
    def upper_boundary; @world.height - @zoom_width/2.0 - 1; end

    def move(x, y, z)
        @center      = [@center[0]+x, @center[1]+y]
        @zoom_width += z

        if self.camera.ratio >= 1
            if @zoom_width > (@world.height-1)
                @zoom_width = (@world.height-1)
            elsif @zoom_width < 1
                @zoom_width = 1
            end
        else
            if @zoom_width > (@world.width-1)
                @zoom_width = (@world.width-1)
            elsif @zoom_width < 1
                @zoom_width = 1
            end
        end

        if @center[0] < self.left_boundary
            @center[0] = self.left_boundary
        elsif @center[0] > self.right_boundary
            @center[0] = self.right_boundary
        end

        if @center[1] < self.lower_boundary
            @center[1] = self.lower_boundary
        elsif @center[1] > self.upper_boundary
            @center[1] = self.upper_boundary
        end

        recenter
    end
end

class IsoCamera < Camera
    def initialize(name, world)
        super(name, world)

        @position = [0.5*@world.width, 0.0,               @world.width*0.5 + @world.depth*0.5]
        @focus    = [0.5*@world.width, 0.5*@world.height, 0.0                                ]
        
        if @camera.nil?
            $logger.error "IsoCamera failed to initialize"
        else
            @camera.set_fixed_yaw(0, 0, 1)
            @camera.set_position(*@position)
            @camera.look_at(*@focus)
        end
    end

    # We need a more intelligent way of doing this
    def adjust_pitch(amt)
        @camera.adjust_pitch(amt)
    end

    def adjust_yaw(amt)
        @camera.adjust_yaw(amt)
    end

    def move(x, y, z)
        @camera.move_relative(x,y,z)
        #@camera.move_absolute(x, y, z)
    end
end
