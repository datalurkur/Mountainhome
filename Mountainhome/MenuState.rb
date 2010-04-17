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
        Console.new(@manager) do |text|
            eval(text)
        end

        # This element is used to group elements of a menu together for easy
        #  deletion when switching to a different menu
        @t_root = Parent.build("transitory", @manager)

        setup_persistent_elements
        setup_top_menu
    end

    def setup_persistent_elements
        # Elements that we want to persist across menu transitions should be created here
        Image.build("mhtitleimg", @manager, "mh-title", @manager.width/2, @manager.height/2, 512, 512)
    end
    
    def setup_top_menu
        @t_root.cull_children

        Title.build("mhtitle", @manager, "MOUNTAINHOME", 100, @manager.height-100, 150, 20, {:parent => @t_root})

        Button.build("create", @manager, "Generate World", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            # Go to create world screen
            setup_create_menu
        end
        Button.build("load", @manager, "Load World", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            # Go to loading screen
            setup_load_menu
        end
        Button.build("options", @manager, "Options / Keys", 100, @manager.height-220, 150, 20, {:parent => @t_root}) do
            # Go to options menu
            setup_options_menu
        end
        Button.build("quit", @manager, "Quit", 100, @manager.height-260, 150, 20, {:parent => @t_root}) do
            # Exit the game
            @core.exit
        end
    end

    def setup_create_menu
        @t_root.cull_children

        Title.build("gentitle", @manager, "GENERATE", 100, @manager.height-100, 150, 20, {:parent => @t_root})

        Button.build("generate", @manager, "Generate!", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            @core.set_state("LoadingState")
        end
        Button.build("back", @manager, "Back to Main Menu", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end
    end

    def setup_load_menu
        @t_root.cull_children

        Title.build("loadtitle", @manager, "LOAD", 100, @manager.height-100, 150, 20, {:parent => @t_root})

        Button.build("load", @manager, "Load", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            # Load saved world
        end
        Button.build("back", @manager, "Back to Main Menu", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end
    end

    def setup_options_menu
        @t_root.cull_children

        Title.build("optionstitle", @manager, "OPTIONS", 100, @manager.height-100, 150, 20, {:parent => @t_root})

        Button.build("back", @manager, "Back to Main Menu", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            setup_top_menu
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
