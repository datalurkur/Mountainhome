require 'UIManager'
require 'World'

class GameState < MHState
    def initialize(core)
        @core = core
    end

    def setup(world)
        @world = world
        @manager = UIManager.new("playing", @core)

        # Attach everything to the window before adding the UI stuff.
        @core.window.set_bg_color(0.4, 0.6, 0.8)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.camera, 0)
        view.add_source(@manager, 1)

        # Add the actual UI elements.
        @console = Console.new(@manager) do |text|
            eval(text)
        end
    end

    def update(elapsed)
        @manager.update(elapsed)
        @world.update(elapsed)
    end

    def teardown
        @core.window.clear_viewports
    end

    def key_pressed(key, modifier)
        args = {:type => :keyboard, :state => :pressed, :key => key, :modifier => modifier}

        status = :handled
        case key
        when Keyboard.KEY_BACKQUOTE
            @console.toggle
        when Keyboard.KEY_TAB
            @mouselook = !@mouselook
        else
            status = :unhandled
        end

        status = @manager.input_event(args) if status == :unhandled
        status = @world.input_event(args) if status == :unhandled

        case key
        when Keyboard.KEY_q
            self.teardown
        end if status == :unhandled
    end

    def key_released(key, modifier)
        args = {:type => :keyboard, :state => :released, :key => key, :modifier => modifier}
        status = @manager.input_event(args)
        status = @world.input_event(args) if status == :unhandled
    end

    def mouse_moved(absX, absY, relX, relY)
        callee = @mouselook ? @world : @manager
		callee.input_event({:type => :move, :abs_x => absX, :abs_y => absY, :x => relX, :y => relY})
    end

    def mouse_pressed(button, x, y)
        @manager.input_event({:type => :mouse, :button => button, :state => :pressed, :x => x, :y => y})
    end

    def mouse_released(button, x, y)
        @manager.input_event({:type => :mouse, :button => button, :state => :released, :x => x, :y => y})
    end
end
