require 'World'
require 'UIManager'
require 'ActionPack'

class GameState < MHState
    def initialize(core)
        @core = core

        # Create a new pack with keybindings registered in GameStateAP.rb.
        @ap = ActionPack.new("GameStateAP")

        # Set some default actions; these have to be defined in GameState scope
        @ap.register_action(:toggle_console) { @console.toggle }
        @ap.register_action(:toggle_mouselook) { @uimanager.toggle_cursor; @world.toggle_mouselook }
        @ap.register_action(:toggle_filled) { @wireframe = !@wireframe }
        @ap.register_action(:escape) { @core.set_state("MenuState") }
        @ap.register_action(:cycle_camera) {
            new_cam = @world.cameras.first
            $logger.info "Switched active camera to #{new_cam.class}"

            if @world.active_camera.class == FirstPersonCamera
                @fp_actor.visible = true
                @fp_actor = nil
            elsif new_cam.class == FirstPersonCamera
                # This will depend on the current selection later
                # TODO
                @fp_actor = @world.actors.first

                new_cam.actor = @fp_actor
                @fp_actor.visible = false
            end

            new_cam.set_active
            @world.cameras = @world.cameras[1...@world.cameras.size] << new_cam

            @core.window.clear_viewports
            view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
            view.add_source(@world.active_camera.camera, 0)
            view.add_source(@uimanager, 1)
        }
        @ap.register_action(:increase_depth) {
            @world.active_camera.change_depth(1) if @world.active_camera.respond_to?(:change_depth)
        }
        @ap.register_action(:decrease_depth) {
            @world.active_camera.change_depth(-1) if @world.active_camera.respond_to?(:change_depth)
        }

        @ap.register_action(:open_job_menu) {
            if @job_menu.nil?
                @job_menu = @uimanager.create(ElementGroup, {:parent=>@uimanager.root, :element_size=>[16,2],
                    :snap=>[:left,:bottom], :ldims=>[1,2,12,12], :grouping=>:square_grid})
                @job_menu.add_element @uimanager.create(Button, {:text=>"Mine Random"}) {
                    mine_random
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                @job_menu.add_element @uimanager.create(Button, {:text=>"All Mine Random"}) {
                    @world.actors.each { |a| mine_random if a.class.include?(Worker)}
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                @job_menu.add_element @uimanager.create(Button, {:text=>"Move Random"}) {
                    move_random
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                @job_menu.add_element @uimanager.create(Button, {:text=>"All Move Random"}) {
                    @world.actors.each { |a| move_random if a.class.include?(Movement)}
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
            end
        }

        @ap.register_action(:open_dwarves_menu) {
            if @dwarves_menu.nil?
                @dwarves_menu = @uimanager.create(ElementGroup, {:parent=>@uimanager.root, :element_size=>[16,2],
                    :snap=>[:left,:bottom], :ldims=>[1,2,12,12], :grouping=>:square_grid})

                @world.actors.each do |actor|
                    if actor.class == Dwarf
                        @dwarves_menu.add_element @uimanager.create(Button, {:text=>actor.name}) {
                            @picker.make_selection([actor])
                            @uimanager.kill_element(@dwarves_menu); @dwarves_menu = nil
                        }
                    end
                end
            end
        }

        @ap.register_action(:open_save_dialog) {
            @uimanager.create(InputDialog, {:parent=>@uimanager.root, :text=>"Save world as..."}) { |filename|
                if filename.length > 0
                    @world.save(@core.personal_directory + filename)
                    :accept
                else
                    @uimanager.create(InfoDialog, {:parent=>@uimanager.root, :text=>"Please enter a filename."})
                    :reject
                end
            }
        }
    end

    def setup(world)
        @world = world
        @uimanager = UIManager.new("playing", @core)
        @jobmanager = JobManager.new(@world)
        @reticle = Reticle.new(world)
        @picker = Picker.new(@uimanager, @world)

        ##
        # And some default events to trigger those actions. This will eventually
        # go away in favor of a GameOptions setter of some sort.
        ##
        @ap.register_event(:toggle_console,   KeyPressed.new(Keyboard.KEY_BACKQUOTE))
        @ap.register_event(:toggle_mouselook, KeyPressed.new(Keyboard.KEY_TAB))
        # Toggle between wireframe and filled when spacebar is pressed.
        @ap.register_event(:toggle_filled,    KeyPressed.new(Keyboard.KEY_SPACE))

        # Return to MenuState
        @ap.register_event(:escape,           KeyPressed.new(Keyboard.KEY_q))

        # Camera controls
        @ap.register_event(:cycle_camera,     KeyPressed.new(Keyboard.KEY_c))
        @ap.register_event(:increase_depth,   KeyPressed.new(Keyboard.KEY_PAGEDOWN))
        @ap.register_event(:decrease_depth,   KeyPressed.new(Keyboard.KEY_PAGEUP))

        Event.add_listeners(@uimanager, @ap, @world, @reticle, @picker)

        # Attach everything to the window before adding the UI stuff.
        @core.window.set_bg_color(0.2, 0.2, 0.2)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.active_camera.camera, 0)
        view.add_source(@uimanager, 1)

        # Add the actual UI elements.
        @console = @uimanager.create(Console, {:parent => @uimanager.root}) { |text| $logger.info "Eval-ing #{text}"; eval(text) }

        hud_tray = @uimanager.create(ElementContainer, {:parent=>@uimanager.root, :ldims=>[0,0,@uimanager.looknfeel.full,1], :snap=>[:bottom,:left], :grouping=>:row})
        hud_tray.add_element @uimanager.create(Button, {:text=>"Save World"}) {
            @ap.call_action(:open_save_dialog)
        }
        hud_tray.add_element @uimanager.create(Button, {:text=>"Dwarves"}) {
            @ap.call_action(:open_dwarves_menu)
        }
        hud_tray.add_element @uimanager.create(Button, {:text=>"Jobs"}) {
            @ap.call_action(:open_job_menu)
        }
        hud_tray.add_element @uimanager.create(Button, {:text=>"Quit to Menu"}) {
            @ap.call_action(:escape)
        }

        # DEBUG CODE
        # Add some test entities
        $logger.info "[+] Creating a test actor"
        @world.actors = []

        actor = create(Dwarf, "Franzibald")
        actor.set_position(0, 0, @world.terrain.get_surface(0,0))
        actor = create(Dwarf, "Sheila")
        actor.set_position(0, 2, @world.terrain.get_surface(0,2))

        # Invoke the managers
        $logger.info "[+] Invoking managers' seed methods"
        MountainhomeDSL.managers.each_value do |manager|
            manager.clear
            manager.seed(@world) if manager.respond_to? :seed
        end

        @core.stop_the_music
    end

    # Actor and MHEntity creation.
    def create(klass, name)
        actor = @world.create(klass, name)
        # Tell the job manager about the new Worker it needs to maintain.
        if klass.include?(Worker)
            @jobmanager.register_worker(actor)
        end
        actor
    end

    # Testing purposes only.
    def move_random
        begin
            x = rand(@world.terrain.width)
            y = rand(@world.terrain.height)
            z = @world.terrain.get_surface(x, y)+1
        end until !@world.terrain.out_of_bounds?(x, y, z)

        @jobmanager.add_job(Move, [x,y,z])
    end

    def mine_random
        begin
            x = rand(@world.terrain.width)
            y = rand(@world.terrain.height)
            z = @world.terrain.get_surface(x, y)
        end until !@world.terrain.out_of_bounds?(x, y, z)

        @jobmanager.add_job(Mine, [x,y,z])
    end

    def update(elapsed)
        @core.render_context.send(@wireframe ? :set_wireframe : :set_filled )
        @uimanager.update(elapsed)
        @world.update(elapsed)
    end

    def teardown
        @core.window.clear_viewports
        Event.remove_listeners(@uimanager, @ap, @world, @reticle)
    end
end

class Picker
    def initialize(uimanager, world)
        @uimanager = uimanager
        @world     = world
    end

    def make_selection(selected_group)
        unless @selection_list.nil?
            @uimanager.kill_element(@selection_list)
        end

        # Display information about the selected objects onscreen
        @selection_list = @uimanager.create(ElementContainer, {
            :parent => @uimanager.root,
            :snap=>[:right, :bottom],
            :ldims=>[-2,2,10,10],
            :grouping=>:column})
        selected_group.each do |actor|
            @selection_list.add_element @uimanager.create(Pane, :text=>"#{actor.name}::#{actor.inspect}")
        end
    end

    def input_event(event)
        case event
        when MousePressed
            @start = [@uimanager.mouse.x, @uimanager.mouse.y]

        when MouseReleased
            # Can happen if the state starts with the mouse down.
            unless @start.nil?
                @end = [@uimanager.mouse.x, @uimanager.mouse.y]

                # Translate the coordinates into frustum space
                [@start, @end].each do |pair|
                    pair[0] = (pair[0] / @uimanager.width.to_f)
                    pair[1] = (pair[1] / @uimanager.height.to_f)
                end

                # Do picking
                selection = @world.pick_objects(@world.active_camera.camera, @start[0], @start[1], @end[0], @end[1])
                selected_group = []
                selection.each_actor { |actor| selected_group << actor }
                make_selection(selected_group)

                @start = nil
                @end   = nil
            end
        end
        return :unhandled
    end
end
