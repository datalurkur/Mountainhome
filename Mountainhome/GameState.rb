require 'World'
require 'UIManager'
require 'EventTranslator'

class GameState < MHState
    def initialize(core)
        @core = core
    end

    def setup(world)
        @world = world
        @manager = UIManager.new("playing", @core)

        @evt = EventTranslator.new
        
        # Set some default actions; these have to be defined in GameState scope
        @evt.set_action(:toggle_console) { @console.toggle }
        @evt.set_action(:toggle_mouselook) { @mouselook = !@mouselook }
        @evt.set_action(:escape) { @core.set_state("MenuState") }
        @evt.set_action(:toggle_filled) do
            @wireframe = !@wireframe
        end

        # And some default events to trigger those actions. This will eventually
        # go away in favor of a GameOptions setter of some sort.
        @evt.set_event(:toggle_console,   basic_keypress(Keyboard.KEY_BACKQUOTE))
        @evt.set_event(:toggle_mouselook, basic_keypress(Keyboard.KEY_TAB))

        # Not sure why this is defined at all... should we return to a menu here?
        @evt.set_event(:escape,           basic_keypress(Keyboard.KEY_q))

        # Toggle between wireframe and filled when spacebar is pressed.
        @evt.set_event(:toggle_filled,    basic_keypress(Keyboard.KEY_SPACE))

        # If the console is enabled, need to pass all keys to it FIRST.
        @evt.default_before_action do |event|
            if @console.toggled? && event[:key] != nil && event[:state] == :pressed
                @console.input_event(event)
            end
        end

        # If there's no action defined for the event, pass it to manager and then world until it's handled.
        @evt.default_after_action do |event|
          status = @manager.input_event(event)
          @world.input_event(event) if status == :unhandled
        end


        # Attach everything to the window before adding the UI stuff.
        @core.window.set_bg_color(0.4, 0.6, 0.8)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.camera, 0)
        view.add_source(@manager, 1)

        # Add the actual UI elements.
        @console = Console.new(@manager, {:parent => @manager.root}) do |text|
            eval(text)
        end

        @core.stop_the_music
    end

    def update(elapsed)
        @core.render_context.send(@wireframe ? :set_wireframe : :set_filled )
        @manager.update(elapsed)
        @world.update(elapsed)
    end

    def teardown
        @core.window.clear_viewports
    end

    def key_pressed(key, modifier)
        @evt.translate({:type => :keyboard, :state => :pressed, :key => key, :modifier => modifier})
    end

    def key_released(key, modifier)
        @evt.translate({:type => :keyboard, :state => :released, :key => key, :modifier => modifier})
    end

    def mouse_moved(absX, absY, relX, relY)
        callee = @mouselook ? @world : @manager
		callee.input_event({:type => :move, :abs_x => absX, :abs_y => absY, :x => relX, :y => relY})
    end

    def mouse_pressed(button, x, y)
        @evt.translate({:type => :mouse, :button => button, :state => :pressed, :x => x, :y => y})
    end

    def mouse_released(button, x, y)
        @evt.translate({:type => :mouse, :button => button, :state => :released, :x => x, :y => y})
    end
end
