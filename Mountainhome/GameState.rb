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
                element_group = []
                element_group << @uimanager.create(Button, {:text=>"Mine Random"}) {
                    # TODO - Insert job creation code here
                    # For now, just tell the actors to mine randomly
                    target = @world.actors.first
                    target.mine_random if target.respond_to? :mine_random
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                element_group << @uimanager.create(Button, {:text=>"All Mine Random"}) {
                    @world.actors.each { |a| a.mine_random if a.respond_to? :mine_random }
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                element_group << @uimanager.create(Button, {:text=>"Move Random"}) {
                    target = @world.actors.first
                    target.move_random if target.respond_to? :move_random
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                element_group << @uimanager.create(Button, {:text=>"All Move Random"}) {
                    @world.actors.each { |a| a.move_random if a.respond_to? :move_random }
                    @uimanager.kill_element(@job_menu); @job_menu = nil
                }
                @job_menu = @uimanager.create(ElementGroup,
                    {:parent=>@uimanager.root, :snap=>[:left,:bottom], :ldims=>[1,5,4,6],
                     :grouping=>:column, :elements=>element_group})
            end
        }
    end

    def setup(world)
        @world = world
        @uimanager = UIManager.new("playing", @core)
        @reticle = Reticle.new(world)
        @picker = Picker.new(@uimanager, @world)

        Event.add_listeners(@uimanager, @ap, @world, @reticle, @picker)

        # Attach everything to the window before adding the UI stuff.
        @core.window.set_bg_color(0.2, 0.2, 0.2)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.active_camera.camera, 0)
        view.add_source(@uimanager, 1)

        # Add the actual UI elements.
        @console = @uimanager.create(Console, {:parent => @uimanager.root}) { |text| $logger.info "Eval-ing #{text}"; eval(text) }

        @uimanager.create(Button, {:parent=>@uimanager.root, :text=>"Save World", :snap=>[:left,:bottom], :ldims=>[1,1,4,1]}) {
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

        # DEBUG CODE
        # Add some test entities
        $logger.info "[+] Creating a test actor"
        @world.actors = []

        # Dwarves currently have a radius of 0.4, so offset the zposition by this much.
        actor = @world.create(Dwarf, "Franzibald")
        actor.set_position(0, 0, @world.terrain.get_surface(0,0))

        actor = @world.create(Dwarf, "Sheila")
        actor.set_position(0, 2, @world.terrain.get_surface(0,2))

        # Invoke the managers
        $logger.info "[+] Invoking managers' seed methods"
        MountainhomeDSL.managers.each_value do |manager|
            manager.clear
            manager.seed(@world) if manager.respond_to? :seed
        end

        @core.stop_the_music
    end

    def update(elapsed)
        # @world.actors.first.move_random
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
                $logger.info "Picking objects from #{@start.inspect} to #{@end.inspect}"
                @selection = @world.pick_objects(@world.active_camera.camera, @start[0], @start[1], @end[0], @end[1])

                # TEMP CODE
                unless @selection_list.nil?
                    @uimanager.kill_element(@selection_list)
                end

                # Display information about the selected objects onscreen
                selected_group = []
                @selection.each_actor { |actor|
                    selected_group << @uimanager.create(Pane, {:text => "#{actor.name} #{actor.inspect}"})
                }
                @selection_list = @uimanager.create(ElementGroup, {
                    :parent => @uimanager.root,
                    :snap=>[:right, :bottom],
                    :ldims=>[-2,1,10,10],
                    :grouping=>:column,
                    :elements=>selected_group})
            end
        end
        return :unhandled
    end
end
