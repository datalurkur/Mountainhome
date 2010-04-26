require 'TerrainBuilder'
require 'UIManager'
require 'World'

class MenuState < MHState
    include StateEventCreator

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
        Image.new("title", @manager, "mh-title.material", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})
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
        Image.new("mhgenimg", @manager, "mh-gen.material", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

        Button.new("generate_small", @manager, "Small World", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            @core.set_state("LoadingState", :generate, {:width => 33, :height => 33, :depth => 17})
        end
        Button.new("generate_large", @manager, "Large World", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            @core.set_state("LoadingState", :generate, {:width => 129, :height => 129, :depth => 65})
        end
        Button.new("generate_custom", @manager, "Custom World", 100, @manager.height-220, 150, 20, {:parent => @t_root}) do
            @core.set_state("LoadingState", :generate, {:width => @custom_breadth, :height => @custom_breadth, :depth => @custom_depth})
        end

        # Custom worldgen configuration
        dims_list = [5, 9, 17, 33, 65, 129, 257, 513]
        @custom_breadth = 65
        @custom_depth   = 17
        Text.new("breadth", @manager, "Breadth", 150, @manager.height-260, {:parent => @t_root})
        TickSlider.new("breadth", @manager, @custom_breadth, dims_list, 150, @manager.height-280, 250, 20, {:parent => @t_root}) do |value|
            @custom_breadth = value.to_i
        end
        Text.new("depth", @manager, "Depth", 150, @manager.height-320, {:parent => @t_root})
        TickSlider.new("depth", @manager, @custom_depth, dims_list, 150, @manager.height-340, 250, 20, {:parent => @t_root}) do |value|
            @custom_depth = value.to_i
        end

        Button.new("back", @manager, "Back to Main Menu", 100, @manager.height-420, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end
    end

    def setup_load_menu
        @t_root.cull_children

        Text.new("loadtitle", @manager, "LOAD", 100, @manager.height-100, {:parent => @t_root, :font => "big.font"})
        Image.new("mhloadimg", @manager, "mh-load.material", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

        @current_dir = ""
        world_list = get_loadable_worlds(@current_dir)
        @loader = ListSelection.new("world_list", @manager, ".../#{@current_dir}", world_list,
                                    @manager.width-250, @manager.height-400, 200, 200, {:parent => @t_root}) do |select|
            @current_dir = change_dir(@loader.list[select])
            @loader.label.text = ".../#{@current_dir}"
            @loader.list = get_loadable_worlds(@current_dir)
        end

        Button.new("load", @manager, "Load", 100, @manager.height-140, 150, 20, {:parent => @t_root}) do
            # Make sure a proper world file is selected
            split_extension = @current_dir.split(".")
            if split_extension.size <= 1
                InfoDialog.new("bad_file", @manager, "Please select a world to load", @manager.width/2, @manager.height/2, 300, 150, {:parent => @t_root})
            elsif split_extension.last != "mhw"
                InfoDialog.new("bad_file", @manager, "Please select a valid MHW file", @manager.width/2, @manager.height/2, 300, 150, {:parent => @t_root})
            else
                # Load saved world
                $logger.info "Loading world with split extension #{split_extension}"
                world_name = split_extension[0...-1].join("/")
                @core.set_state("LoadingState", :load, {:filename => @core.personal_directory + world_name})
            end
         end
        Button.new("back", @manager, "Back to Main Menu", 100, @manager.height-180, 150, 20, {:parent => @t_root}) do
            setup_top_menu
        end
    end

    def setup_options_menu
        @t_root.cull_children

        Text.new("optionstitle", @manager, "OPTIONS", 100, @manager.height-100, {:parent => @t_root, :font => "big.font"})
        Image.new("mhoptionsimg", @manager, "mh-options.material", @manager.width/2, @manager.height/2, 512, 512, {:parent => @t_root})

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

    def change_dir(current_dir)
        # Clean up the path
        dir_pieces = current_dir.split("/")

        new_dir = []
        dir_pieces.each do |piece|
            if piece == ".." && new_dir.size > 1
                new_dir = new_dir[0...-1]
            elsif piece != ".."
                new_dir << piece
            end
        end
        new_dir = new_dir.join("/")
    end

    def get_loadable_worlds(relative_path)
        search_dir = @core.personal_directory + relative_path
        if search_dir.split(".").size > 1
            search_dir = search_dir.split("/")[0...-1].join("/")
        end

        entries = Dir.entries(search_dir)
        entries.reject! do |ent|
            (ent == "." || (ent.split(".").size > 1 and ent.split(".").last != "mhw"))
        end
    end

    def update(elapsed)
        @manager.update(elapsed)
    end

    def teardown
        @core.window.clear_viewports
        @manager.clear_elements
    end
    
    def mouse_moved(event)
		@manager.input_event(event)
    end

    def mouse_pressed(event)
        @manager.input_event(event)
    end

    def mouse_released(event)
        @manager.input_event(event)
    end
end
