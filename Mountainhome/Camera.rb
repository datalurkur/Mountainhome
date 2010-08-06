class Camera
    attr_accessor :camera, :name

    def initialize(name, world)
        @name  = name
        @world = world

        if @world.nil?
            raise RuntimeError, "Camera requires a valid World object!"
        end

        @camera = @world.create_camera(@name)
    end

    def set_active
        @world.active_camera = self
    end
end

class TopCamera < Camera
    def initialize(name, world)
        super(name, world)

        # Start centered at the max z leve;
        @center  = [@world.width * 0.5, @world.height * 0.5]
        @z_level = @world.depth

        # Start zoomed out, for loading. Once updates start happening move will be called
        # and the world will be zoomed in.
        @zoom_width = self.max_zoom_width * 2

        self.recenter
    end

    def recenter
        # XXXBMW: I've never seen anyone use negative near values. I THINK we're safe,
        # though, because we're setting up an ortho projection. May want to double check.
        @camera.center_ortho(@zoom_width, @center[0], @center[1], -@z_level, 0.0)
    end

    def change_depth(mod)
        @z_level = [[@z_level + mod, 0].max, @world.depth].max
        self.recenter
    end

    def move(x, y, z)
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
        (self.camera.ratio > self.world_ratio) ?
            @world.height * self.camera.ratio  :
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
    def zoom_height;    @zoom_width / self.camera.ratio    end
end

class IsoCamera < Camera
    delegate_to :camera, :adjust_pitch, :adjust_yaw, :move

    def initialize(name, world)
        super(name, world)

        @position = [0.5*@world.width, 0.0,               @world.width*0.5 + @world.depth*0.5]
        @focus    = [0.5*@world.width, 0.5*@world.height, 0.0                                ]

        @camera.set_fixed_yaw(0, 0, 1)
        @camera.set_position(*@position)
        @camera.look_at(*@focus)
    end
end
