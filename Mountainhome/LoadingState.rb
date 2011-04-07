require 'TerrainBuilder'
require 'World'

class LoadingState < MHState
    def initialize(core)
        @core = core

        # Create the UIManager and kill the mouse element.
        @uimanager = UIManager.new

        # Add our loading notice.
#        $logger.info "Creating title"
#        @title = @uimanager.create(Title, {:parent=>@uimanager.root, :text_align=>[:left, :center], :text=>"Loading...", :ldims=>[2,2]})
    end

    def setup(action = :generate, args={})
        # Create the world.
        @action = action
        @args = args 
        @frame = 0
    end

    def draw
        @core.render_context.set_viewport(0, 0, @core.window.width, @core.window.height)
        @core.render_context.clear(0.0, 0.0, 0.0, 1.0)
        @world.render(@core.render_context) if @world
    end

    def update(elapsed)
        case @frame
        when 0 then @frame+=1; # Render black on frame 0
        when 1 then @frame+=1; @world = World.new(@core, @action, @args)
        when 2 then @frame+=1; @world.populate()
        else
            # When the builder fiber is done, switch to GameState.
            begin
                @world.builder_fiber.resume
            rescue FiberError
                @core.set_state("GameState", @world)
                @world = nil
            end
        end
    end

    def teardown
    end
end
