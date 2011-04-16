require 'TerrainBuilder'
require 'World'

class LoadingState < MHState
    def initialize(core)
        @core = core
    end

    def setup(action = :generate, args={})
        # Create the world.
        @action = action
        @args = args 
        @frame = 0

        # Create the UIManager.
        @uimanager = UIManager.new

        # Add our loading notice.
        @uimanager.create(Title, {:text=>"Loading...", :lay_pos=>[6,6]})
    end

    def draw
        @core.render_context.set_viewport(0, 0, @core.window.width, @core.window.height)
        @core.render_context.clear(0.0, 0.0, 0.0, 1.0)
        @world.render(@core.render_context) if @world
        @uimanager.render(@core.render_context)
    end

    def update(elapsed)
        @uimanager.update(elapsed)

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
        @world = nil
        @uimanager = nil
        Event.clear_listeners
    end
end
