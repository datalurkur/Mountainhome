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

        Text.new("title", @manager, "MOUNTAINHOME", 100, @manager.height-100, {:parent => @t_root, :font => "big.font"})
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

        Text.new("gentitle", @manager, "GENERATE", 100, @manager.height-100, {:parent => @t_root, :font => "big.font"})
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

        Text.new("loadtitle", @manager, "LOAD", 100, @manager.height-100, {:parent => @t_root, :font => "big.font"})
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

        Text.new("optionstitle", @manager, "OPTIONS", 100, @manager.height-100, {:parent => @t_root, :font => "big.font"})
        Image.new("mhoptionsimg", @manager, "mh-options", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

        # Options
        Text.new("res_title", @manager, "Resolution", 60, @manager.height-120, {:parent => @t_root})
        DropDown.new("resolution", @manager, @core.options.get("video.resolution"),
                     100, @manager.height-140, 150, 20, {:parent => @t_root}) do |drop_down|
            resolutions = [[1680,1050],[1600,1200],[1280,1024],[1024,768],[800,600],[640,480]]
            resolutions.collect! { |res| res.join("x") }
            resolutions.each_with_index do |res,index|
                Selectable.new(res, @manager, res, drop_down.x, drop_down.y-((index+1)*20), drop_down.w, 20, {:parent => drop_down}) do
                    @core.options.put("video.resolution",res)
                    drop_down.selected(res)
                end
            end
        end

        Text.new("aa_title", @manager, "Anti-Aliasing", 400, @manager.height-120, {:parent => @t_root})
        aa_values = [0,2,4,8,16]
        TickSlider.new("antialiasing", @manager, @core.options.get("video.aasamples").to_i, aa_values,
                       440, @manager.height-140, 150, 20, {:parent => @t_root}) do |value|
            @core.options.put("video.aasamples", value)
        end

        Text.new("fs_title", @manager, "Fullscreen", 220, @manager.height-120, {:parent => @t_root})
        CheckBox.new("fullscreen", @manager, (@core.options.get("video.fullscreen") == 1), 220, @manager.height-140, {:parent => @t_root}) do |state|
            @core.options.put("video.fullscreen", state ? "1" : "0")
        end

        Text.new("vs_title", @manager, "Vertical Sync", 300, @manager.height-120, {:parent => @t_root})
        CheckBox.new("verticalsync", @manager, (@core.options.get("video.vsync") == 1), 300, @manager.height-140, {:parent => @t_root}) do |state|
            @core.options.put("video.vsync", state ? "1" : "0")
        end

        Button.new("apply", @manager, "Apply Settings", 100, @manager.height-340, 150, 20, {:parent => @t_root}) do
            @core.options.apply
        end

        Button.new("back", @manager, "Back to Main Menu", 100, @manager.height-380, 150, 20, {:parent => @t_root}) do
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
