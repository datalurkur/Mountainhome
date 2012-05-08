class TopCamera < MHCamera
    def initialize(name, world)
        super(name, world)

        # Start centered at the max z leve;
        @center  = [self.world.width * 0.5, self.world.height * 0.5]
        @z_level = self.world.depth + 1

        # Start zoomed out, for loading. Once updates start happening move will be called
        # and the world will be zoomed in.
        @zoom_width = self.max_zoom_width * 2

        # Force the camera to resituate itself to avoid odd initial configurations
        self.move_relative(0.0, 0.0, 0.0)
    end

    def recenter
        make_center_ortho(@center[0], @center[1], @zoom_width, self.zoom_height)
    end

    def change_depth(mod)
        @z_level = [[@z_level + mod, 0].max, self.world.depth].min
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
            self.world.height * self.ratio  :
            self.world.width
    end

    # Define the boundries based on the smaller of the current, appropriate zoom amount
    # and the related world dimension. This will confine scrolling to the world, but allow
    # gray area to appear around the edges as needed.
    def left_boundary;  [@zoom_width,      self.world.width ].min / 2.0; end
    def lower_boundary; [self.zoom_height, self.world.height].min / 2.0; end
    def right_boundary; self.world.width  - left_boundary;     end
    def upper_boundary; self.world.height - lower_boundary;    end

    # Helper functions.
    def world_ratio;    self.world.width / self.world.height.to_f; end
    def zoom_height;    @zoom_width / self.ratio    end
end

class BasicCamera < MHCamera
    def initialize(name, world)
        super(name, world)

        fov_d    = 60.0
        h_fov_r  = (fov_d / 2.0) * (Math::PI / 180)
        h_width  = self.world.width / 2.0
        h_height = self.world.height / 2.0
        h_depth  = self.world.depth / 2.0

        # Set the default z level to give us a clear view of the map edges.
        z_level = [h_width, h_height * self.ratio].max / Math::tan(h_fov_r)

        # Zoom out just a bit further to give us some on smaller maps
        z_level += h_depth if h_depth < 4

        # And actually place the camera in the middle of the map looking down.
        set_fixed_yaw(0, 0, 1)
        set_direction(0, 0, -1)
        set_position(h_width, h_height, z_level)
        make_perspective(fov_d)
    end
end
