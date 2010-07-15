require 'TerrainBuilder'
require 'UIManager'
require 'World'

class LoadingState < MHState
    include StateEventCreator

    def initialize(core)
        @core = core

        # Create the UIManager and kill the mouse element.
        @manager = UIManager.new("default", @core)
        @manager.clear_elements(true)

        # Add our loading notice.
        Text.new("loading", @manager, "Loading...", 40, 20, {:font => "big.font", :parent => @manager.root})
    end

    def setup(action = :generate, args={})
        # Create the world.
        case action
        when :generate
            @world = World.new(@core, :generate, args)
        when :load
            @world = World.new(@core, :load, args)
        end

        # Attach the UI to the window BEFORE doing the UI.
        @core.window.set_bg_color(0.0, 0.0, 0.0)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.camera, 0)
        view.add_source(@manager, 1)
        @frame = 0
    end

    def update(elapsed)
        case @frame
        when 0 then @frame+=1; # Render black on frame 0
        when 1 then @frame+=1; @world.populate()
        else
            # When the builder fiber is done, switch to GameState.
            if @world.builder_fiber.resume
                @core.set_state("GameState", @world)
            end
        end
    end

    def teardown
        @core.window.clear_viewports
    end
end
