class Camera < MHCamera
    attr_accessor :name

    def initialize(name, world)
        @name  = name
        @world = world

        if @world.nil?
            raise RuntimeError, "Camera requires a valid World object!"
        end
    end

    def ratio
        # XXXBMW: I do NOT like this. Just trying to get shit working.
        $mhcore.window.ratio
    end
end

class TopCamera < Camera
    def initialize(name, world)
        super(name, world)

        # Start centered at the max z leve;
        @center  = [@world.width * 0.5, @world.height * 0.5]
        @z_level = @world.depth - 1.0

        # Start zoomed out, for loading. Once updates start happening move will be called
        # and the world will be zoomed in.
        @zoom_width = self.max_zoom_width * 2

        # Force the camera to resituate itself to avoid odd initial configurations
        self.move_relative(0.0, 0.0, 0.0)
    end

    def recenter
        center_ortho(@zoom_width, self.zoom_height, @center[0], @center[1], -@z_level, @z_level)
    end

    def change_depth(mod)
        @z_level = [[@z_level + mod, 0].max, @world.depth].max
        self.recenter
    end

    def move_relative(x, y, z)
        @center[0]  = [[@center[0]  + x, self.left_boundary ].max, self.right_boundary].min
        @center[1]  = [[@center[1]  + y, self.lower_boundary].max, self.upper_boundary].min
        @zoom_width = [[@zoom_width + z, 1                  ].max, self.max_zoom_width].min
        self.recenter
    end

protected
    # If the camera ratio is greater than the world ratio, the world height is going to be
    # the contraining factor (and will need to be converted to a width value based on the
    # camera ratio), otherwise the width is the contraining factor.
    def max_zoom_width
        (self.ratio > self.world_ratio) ?
            @world.height * self.ratio  :
            @world.width
    end

    # Define the boundries based on the smaller of the current, appropriate zoom amount
    # and the related world dimension. This will confine scrolling to the world, but allow
    # gray area to appear around the edges as needed.
    def left_boundary;  [@zoom_width,      @world.width ].min / 2.0; end
    def lower_boundary; [self.zoom_height, @world.height].min / 2.0; end
    def right_boundary; @world.width  - left_boundary;     end
    def upper_boundary; @world.height - lower_boundary;    end

    # Helper functions.
    def world_ratio;    @world.width / @world.height.to_f; end
    def zoom_height;    @zoom_width / self.ratio    end
end

class BasicCamera < Camera
    def initialize(name, world)
        super(name, world)

        @position = [0.5*@world.width, 0.0,               @world.width*0.5 + @world.depth*0.5]
        @focus    = [0.5*@world.width, 0.5*@world.height, 0.0                                ]

        set_fixed_yaw(0, 0, 1)
        set_position(*@position)
        look_at(*@focus)
    end
end

class FirstPersonCamera < Camera
    attr_accessor :actor

    def initialize(name, world, actor)
        super(name, world)

        set_fixed_yaw(0, 0, 1)
        look_at(0, 1, 0)

        @actor = actor
    end

    def update
        # Update the position and facing of the camera relative to its actor
        # Separating this update from the move_relative and adjust_<direction> calls
        #  allows us to abstract out the physics from this section of the code

        updated_position = @actor.position

        if updated_position != self.position
            $logger.info "Updating camera position #{self.position.inspect} to #{updated_position.inspect}"
            set_position(*@actor.position)
        end
    end

    def move_relative(x, y, z)
        # Eventually, we'll want to take the vector supplied by move_relative and use
        #  it in conjunction with the direction the entity is facing the move the entity
        #  some small distance using Moveable module's "step" method
        # TODO

        # Move the actor accordingly
        @actor.set_position(*(@actor.position.piecewise_add([x,y,z])))
    end

    def adjust_pitch(value)
        # TODO
        # These will eventually control the direction the entity is facing,
        #  from which the camera will update itself
        self.adjust_pitch(value)
    end

    def adjust_yaw(value)
        # TODO
        # These will eventually control the direction the entity is facing,
        #  from which the camera will update itself
        self.adjust_yaw(value)
    end
end
