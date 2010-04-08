require 'TerrainBuilder'
require 'UIManager'
require 'World'

class MenuState < MHState
    def initialize(core)
        @core = core
    end

    def setup
        # Setup the UI.
        @manager = UIManager.new("default", @core)

        # Attach the UI to the window BEFORE doing the UI.
        @core.window.set_bg_color(0.6, 0.6, 0.6)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@manager, 1)

        # Add the actual UI elements.
        Button.new("create", @manager, "Create New World", @manager.width / 2, @manager.height / 2, 200, 50) do
            @core.window.set_bg_color(0.0, 0.0, 0.0)
            @manager.clear_elements(true)
            @manager.add_element("loading notice", 10, 10, 0, 0, {:text => "Loading...", :font => "big.font"})
            @load = :wait
        end

        Console.new(@manager) do |text|
            eval(text)
        end

    end

    def update(elapsed)
        case @load
        when :wait
            @load = :switch
        when :switch
            world = World.new(129, 129, 129, @core)
            $mhcore.set_state("GameState", world)
        else
            @manager.update(elapsed)
        end
    end

    def teardown
        @core.window.clear_viewports
    end
    
    def mouse_moved(absX, absY, relX, relY)
		@manager.input_event({:type => :move, :abs_x => absX, :abs_y => absY, :x => relX, :y => relY})
    end

    def mouse_pressed(button, x, y)
        @manager.input_event({:type => :mouse, :button => button, :state => :pressed, :x => x, :y => y})
    end

    def mouse_released(button, x, y)
        @manager.input_event({:type => :mouse, :button => button, :state => :released, :x => x, :y => y})
    end
end
