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
        @uimanager = UIManager.new(@core.window.width, @core.window.height)

        # Add our loading notice.
        @loading_notice = @uimanager.create(Title, {:text=>"Prepping for world generation...", :lay_pos=>[1,1]})

        Event.add_listeners(self)

        # Default to forcing no halt. Pressing the correct key will trigger this functionality
        @halt_for_input = false
        @force_no_halt = true
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
        else
            if @force_no_halt || !@halt_for_input
                # When the builder fiber is done, switch to GameState.
                begin
                    @world.builder_fiber.resume

                    # When the fiber returns, we'll be poised just before the currently
                    # set builder step name.
                    @loading_notice.text = "Current step: #{@world.builder_step}"

                rescue FiberError # Assume this means the fiber is finished executing.
                    @core.set_state("GameState", @world)
                    @world = nil
                end

                @halt_for_input = true
            end
        end
    end

    def teardown
        @world = nil
        @uimanager = nil
        Event.clear_listeners
    end

    def input_event(event)
        if event.kind_of?(KeyPressed)
            case event.key
            when Keyboard.KEY_SPACE then @force_no_halt = @halt_for_input = false
            when Keyboard.KEY_c     then @world.cycle_cameras
            end
        end

        return :unhandled
    end
end
