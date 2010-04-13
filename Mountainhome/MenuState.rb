require 'TerrainBuilder'
require 'UIManager'
require 'World'

class MenuState < MHState
    def initialize(core)
        @core = core

        # Create the UIManager and add the main screen elements.
        @manager = UIManager.new("default", @core)
    end

    def setup
        # Attach the UI to the window BEFORE doing the UI.
        @core.window.set_bg_color(0.6, 0.6, 0.6)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@manager, 1)

        # We have to do this here, for now, since width and height are only valid AFTER
        # attaching to a Viewport.
        Button.new("create", @manager, "Create New World", @manager.width / 2, @manager.height / 2, 200, 50) do
            @core.set_state("LoadingState")
        end

        Console.new(@manager) do |text|
            eval(text)
        end
    end

    def update(elapsed)
        @manager.update(elapsed)
    end

    def teardown
        @core.window.clear_viewports
        @manager.clear_elements
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
