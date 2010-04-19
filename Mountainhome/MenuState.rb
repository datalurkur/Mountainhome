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
        Console.new(@manager, {:parent => @manager.root}) { |input_text| eval(input_text) }

        # This element is used to group elements of a menu together for easy
        #  deletion when switching to a different menu
        @t_root = Invisible.new("transitory", @manager, {:parent => @manager.root})

        setup_persistent_elements
        setup_top_menu
    end

    def setup_persistent_elements
        # Elements that we want to persist across menu transitions should be created here
    end
    
    def setup_top_menu
        @t_root.cull_children

        Text.new("title", @manager, "MOUNTAINHOME", 100, @manager.height-100, {:parent => @t_root})
        Image.new("title", @manager, "mh-title", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})
        Button.new("generate", @manager, "Generate World", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            # Go to worldgen screen
            setup_generate_menu
        end
        Button.new("load", @manager, "Load World", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            # Go to loading screen
            setup_load_menu
        end
        Button.new("options", @manager, "Options / Keys", 100, @manager.height-220, 150, 20, {:parent => @t_root}) do
            # Go to options screen
            setup_options_menu
        end
        Button.new("quit", @manager, "Quit", 100, @manager.height-260, 150, 20, {:parent => @t_root}) do
            # Exit the game
            @core.exit
        end
    end

    def setup_generate_menu
        @t_root.cull_children

        Text.new("gentitle", @manager, "GENERATE", 100, @manager.height-100, {:parent => @t_root})
        Image.new("mhgenimg", @manager, "mh-gen", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

        Button.new("generate", @manager, "Generate!", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            @core.set_state("LoadingState")
        end
        Button.new("back", @manager, "Back to Main Menu", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end
    end

    def setup_load_menu
        @t_root.cull_children

        Text.new("loadtitle", @manager, "LOAD", 100, @manager.height-100, {:parent => @t_root})
        Image.new("mhloadimg", @manager, "mh-load", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

        Button.new("load", @manager, "Load", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            # Load saved world
            @core.set_state("LoadingState", :load)
         end
        Button.new("back", @manager, "Back to Main Menu", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end
    end

    def setup_options_menu
        @t_root.cull_children

        Text.new("optionstitle", @manager, "OPTIONS", 100, @manager.height-100, {:parent => @t_root})
        Image.new("mhoptionsimg", @manager, "mh-options", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

        Button.new("back", @manager, "Back to Main Menu", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end

        # TEST CODE
        DropDown.new("testdd", @manager, "Drop Down", 100, @manager.height-220, 150, 20, {:parent => @t_root}) do |drop_down|
            items = ["item1", "item2", "item3"]
            items.each_with_index do |item,index|
                Selectable.new(item, @manager, item, 100, drop_down.y-((index+1)*20), drop_down.w, 20, {:parent => drop_down}) do
                    drop_down.selected(item)
                end
            end
        end

        @sliderval = Text.new("slidervalue", @manager, "", 100, @manager.height-340, {:parent => @t_root})

        Slider.new("testslider", @manager, 0.0, 100, @manager.height-300, 150, 20, {:parent => @t_root}) do |value|
            @sliderval.text = (value*100).to_s
        end
        # /TEST CODE
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
