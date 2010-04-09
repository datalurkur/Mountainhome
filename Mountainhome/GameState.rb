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
        
        # Set up some defaults for EventTranslator.
        @evt = EventTranslator.new
        @evt.set_mapping({:type => :keyboard, :state => :pressed, :key => Keyboard.KEY_BACKQUOTE, :modifier => 0}) do
            @console.toggle
        end
        @evt.set_mapping({:type => :keyboard, :state => :pressed, :key => Keyboard.KEY_TAB, :modifier => 0}) do
            @mouselook = !@mouselook
        end
        # Not sure why this is defined at all... should we return to a menu here?
        @evt.set_mapping({:type => :keyboard, :state => :pressed, :key => Keyboard.KEY_q, :modifier => 0}) do
            self.teardown
        end
        
        # If the console is enabled, need to pass all keys to it first.
        @evt.default_before_action do |event|
            if @console.toggled? && event[:key] != nil && event[:state] == :pressed
                @console.input_event(event)
            end
        end
                
        # If the event doesn't have a mapping, defer to manager and then world as necessary.
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
        @console = Console.new(@manager) do |text|
            eval(text)
        end
    
        @core.stop_the_music
    end

    def update(elapsed)
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
