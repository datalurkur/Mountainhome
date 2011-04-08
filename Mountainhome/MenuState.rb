require 'TerrainBuilder'
require 'UIManager'
require 'World'

class MenuState < MHState
    def initialize(core)
        @core = core
    end

    def setup
        # Create the UIManager and add the main screen elements.
        @uimanager = UIManager.new(@core.window.width, @core.window.height)

        Event.add_listeners(@uimanager)

        @t_root = @uimanager.create(UIPane, {:lay_dims => [$max_dim,$max_dim]})

        # It's silly to create a Console if it's impossible to use.
        #@console = @uimanager.create(Console) { |input_text| eval(input_text) }

        setup_persistent_elements
        setup_top_menu
    end

    def setup_persistent_elements
        # Elements that we want to persist across menu transitions should be created here
    end
    
    def setup_top_menu
        @t_root.delete_children

        @uimanager.create(Title, {:text=>"Mountainhome", :lay_pos=>[1,18]}, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,16], :lay_dims=>[4,1],
            :text=>"Generate World", :on_click=>Proc.new { setup_generate_menu }}, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,14], :lay_dims=>[4,1],
            :text=>"Load World",     :on_click=>Proc.new { setup_load_menu }    }, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,12], :lay_dims=>[4,1],
            :text=>"Options",        :on_click=>Proc.new { setup_options_menu } }, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,10], :lay_dims=>[4,1],
            :text=>"Quit",           :on_click=>Proc.new { @core.exit }         }, @t_root)

        # Title
        #@uimanager.create(UIElement, {:parent=>@t_root, :snap=>[:right,:bottom], :ldims=>[-1,0], :dims=>[512,512]}, "mh-title.material")
    end

    def setup_generate_menu
        @t_root.delete_children

        @uimanager.create(Title, {:text=>"Generate", :lay_pos=>[1,18]}, @t_root)

        # Default world-setups
        @uimanager.create(Button, {:lay_pos=>[1,16], :lay_dims=>[4,1], :text=>"Empty World",  :on_click=>Proc.new{
                @core.set_state("LoadingState", :empty, {:width => 9, :height => 9, :depth => 9})
        }}, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,14], :lay_dims=>[4,1], :text=>"Small World",  :on_click=>Proc.new{
                @core.set_state("LoadingState", :generate, {:width => 17, :height => 17, :depth => 9})
        }}, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,12], :lay_dims=>[4,1], :text=>"Medium World", :on_click=>Proc.new{
                @core.set_state("LoadingState", :generate, {:width => 65, :height => 65, :depth => 33})
        }}, @t_root)
        @uimanager.create(Button, {:lay_pos=>[1,10], :lay_dims=>[4,1], :text=>"Large World",  :on_click=>Proc.new{
                @core.set_state("LoadingState", :generate, {:width => 257, :height => 257, :depth => 65})
        }}, @t_root)

        # Custom-sized world setup
        @uimanager.create(Button, {:lay_pos=>[1,8], :lay_dims=>[4,1], :text=>"Custom World", :on_click=>Proc.new{
                @core.set_state("LoadingState", :generate,{:width=>@custom_breadth, :height=>@custom_breadth, :depth=>@custom_depth})
        }}, @t_root)

        @uimanager.create(Label,  {:text=>"Custom Breadth", :lay_pos=>[6,8]}, @t_root)
        @uimanager.create(Slider, {:lay_pos=>[6,7], :lay_dims=>[4,1],
            :slider_values=>[33,65,129,257,513], :on_set=>Proc.new{ |val| @custom_breadth = val }}, @t_root)
        @uimanager.create(Label,  {:text=>"Custom Depth", :lay_pos=>[6,6]}, @t_root)
        @uimanager.create(Slider, {:lay_pos=>[6,5], :lay_dims=>[4,1],
            :slider_values=>[33,65,129,257,513], :on_set=>Proc.new{ |val| @custom_depth = val }}, @t_root)

        # Back to main menu
        @uimanager.create(Button, {:lay_pos=>[1,4], :lay_dims=>[4,1], :text=>"Back", :on_click=>Proc.new{ setup_top_menu }}, @t_root)

        #@uimanager.create(UIElement, {:parent=>@t_root, :snap=>[:right,:bottom], :ldims=>[-1,0], :dims=>[512,512]}, "mh-gen.material")
    end

    def setup_load_menu
        @t_root.delete_children
=begin

        @uimanager.create(UIElement, {:parent=>@t_root, :snap=>[:right,:bottom], :ldims=>[-1,0], :dims=>[512,512]}, "mh-load.material")
        @uimanager.create(Title,  {:text=>"Load", :parent=>@t_root, :ldims=>[1,14,0,0]})
        @uimanager.create(Button, {:text=>"Back", :parent=>@t_root, :ldims=>[1,2,4,1]}) { setup_top_menu }

        @current_dir = ""
        world_list = get_loadable_worlds(@current_dir)
        @loader = @uimanager.create(ListSelection, {:parent=>@t_root, :ldims=>[1,12,6,6],
                                                  :text=>".../#{@current_dir}", :list=>world_list}) { |select|
            @current_dir = change_dir(@loader.list[select])
            @loader.text = ".../#{@current_dir}"
            @loader.list = get_loadable_worlds(@current_dir)
        }
        @uimanager.create(Button, {:text=>"Load", :parent=>@t_root, :ldims=>[1,6,4,1]}) {
            # Make sure a proper world file is selected
            split_extension = @current_dir.split(".")
            if split_extension.size <= 1
                @uimanager.create(InfoDialog, {:parent=>@t_root, :text=>"Please select a world to load."})
            elsif split_extension.last != "mhw"
                @uimanager.create(InfoDialog, {:parent=>@t_root, :text=>"Please select a valid MHW file."})
            else
                # Load saved world
                $logger.info "Loading world with split extension #{split_extension}"
                world_name = split_extension[0...-1].join("/")
                @core.set_state("LoadingState", :load, {:filename => @core.personal_directory + world_name})
            end
        }
=end
    end

    def setup_options_menu
        @t_root.delete_children
=begin

        @uimanager.create(UIElement, {:parent=>@t_root, :snap=>[:right,:bottom], :ldims=>[-1,0], :dims=>[512,512]}, "mh-options.material")
        @uimanager.create(Title,  {:text=>"Options", :parent=>@t_root, :ldims=>[1,16]})

        @uimanager.create(Text, {:parent=>@t_root, :ldims=>[1,14], :text=>"Resolution"})
        resolutions = ["1680x1050","1600x1200","1280x1024","1024x768","800x600","640x480"]
        @uimanager.create(DropDown, {:parent=>@t_root, :ldims=>[4,14,4,1], :list=>resolutions, :default=>@core.options.get("video.resolution")}, "white") { |res| @core.options.put("video.resolution",res) }

        @uimanager.create(Text, {:parent=>@t_root, :ldims=>[1,12], :text=>"Anti-Aliasing"})
        @uimanager.create(TickSlider, {:parent=>@t_root, :ldims=>[4,12,4,1], :default=>@core.options.get("video.aasamples").to_i, :values=>[0,2,4,8,16]}) { |value|
            @core.options.put("video.aasamples", value.to_s)
        }

        @uimanager.create(Text, {:parent=>@t_root, :ldims=>[1,10], :text=>"Fullscreen"})
        @uimanager.create(CheckBox, {:parent=>@t_root, :ldims=>[4,10]}) { |state| @core.options.put("video.fullscreen", state ? "1" : "0") }
        @uimanager.create(Text, {:parent=>@t_root, :ldims=>[1,8], :text=>"Vertical Sync"})
        @uimanager.create(CheckBox, {:parent=>@t_root, :ldims=>[4,8]}) { |state| @core.options.put("video.vsync", state ? "1" : "0") }

        @uimanager.create(Button, {:text=>"Apply Settings", :parent=>@t_root, :ldims=>[1,4,4,1]}) { @core.options.apply }
        @uimanager.create(Button, {:text=>"Back", :parent=>@t_root, :ldims=>[1,2,4,1]}) { setup_top_menu }
=end
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

    def draw
        # @core.render_context.send(:set_filled)
        @core.render_context.set_viewport(0, 0, @core.window.width, @core.window.height)
        @core.render_context.clear(0.2, 0.2, 0.2, 1.0)

        @uimanager.render(@core.render_context)
    end

    def update(elapsed)
        @uimanager.update(elapsed)
    end

    def teardown
        Event.remove_listeners(@uimanager)
    end
end
