require 'TerrainBuilder'
require 'UIManager'
require 'World'

class LoadingState < MHState
    def initialize(core)
        @core = core

        # Create the UIManager and kill the mouse element.
        @uimanager = UIManager.new("default", @core)
        @uimanager.clear_elements(true)

        # Add our loading notice.
        $logger.info "Creating title"
        @title = @uimanager.create(Title, {:parent=>@uimanager.root, :text_align=>[:left, :center], :text=>"Loading...", :ldims=>[2,2]})
    end

    def setup(action = :generate, args={})
        # Create the world.
        @world = World.new(@core, Tile.children, action, args)

        # Attach the UI to the window BEFORE doing the UI.
        @core.window.set_bg_color(0.0, 0.0, 0.0)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.active_camera.camera, 0)
        view.add_source(@uimanager, 1)
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
