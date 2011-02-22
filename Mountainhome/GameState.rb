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
        # UI - @ap.register_action(:toggle_mouselook) { @uimanager.toggle_cursor; @world.toggle_mouselook }
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

            @world.active_camera = new_cam
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

        # UI - @ap.register_action(:open_job_menu) {
        # UI -     if @job_menu.nil?
        # UI -         @job_menu = @uimanager.create(ElementGroup, {:parent=>@uimanager.root, :element_size=>[16,2],
        # UI -             :snap=>[:left,:bottom], :ldims=>[1,2,12,12], :grouping=>:square_grid})
        # UI -         @job_menu.add_element @uimanager.create(Button, {:text=>"Mine Random"}) {
        # UI -             mine_random
        # UI -             @uimanager.kill_element(@job_menu); @job_menu = nil
        # UI -         }
        # UI -         @job_menu.add_element @uimanager.create(Button, {:text=>"All Mine Random"}) {
        # UI -             @world.actors.each { |a| mine_random if a.class.include?(Worker)}
        # UI -             @uimanager.kill_element(@job_menu); @job_menu = nil
        # UI -         }
        # UI -         @job_menu.add_element @uimanager.create(Button, {:text=>"Move Random"}) {
        # UI -             move_random
        # UI -             @uimanager.kill_element(@job_menu); @job_menu = nil
        # UI -         }
        # UI -         @job_menu.add_element @uimanager.create(Button, {:text=>"All Move Random"}) {
        # UI -             @world.actors.each { |a| move_random if a.class.include?(Movement)}
        # UI -             @uimanager.kill_element(@job_menu); @job_menu = nil
        # UI -         }
        # UI -     end
        # UI - }

        # UI - @ap.register_action(:open_dwarves_menu) {
        # UI -     if @dwarves_menu.nil?
        # UI -         @dwarves_menu = @uimanager.create(ElementGroup, {:parent=>@uimanager.root, :element_size=>[16,2],
        # UI -             :snap=>[:left,:bottom], :ldims=>[1,2,12,12], :grouping=>:square_grid})
        # UI - 
        # UI -         @world.actors.each do |actor|
        # UI -             if actor.class == Dwarf
        # UI -                 @dwarves_menu.add_element @uimanager.create(Button, {:text=>actor.name}) {
        # UI -                     @picker.make_selection([actor])
        # UI -                     @uimanager.kill_element(@dwarves_menu); @dwarves_menu = nil
        # UI -                 }
        # UI -             end
        # UI -         end
        # UI -     end
        # UI - }

        # UI - @ap.register_action(:open_save_dialog) {
        # UI -     @uimanager.create(InputDialog, {:parent=>@uimanager.root, :text=>"Save world as..."}) { |filename|
        # UI -         if filename.length > 0
        # UI -             @world.save(@core.personal_directory + filename)
        # UI -             :accept
        # UI -         else
        # UI -             @uimanager.create(InfoDialog, {:parent=>@uimanager.root, :text=>"Please enter a filename."})
        # UI -             :reject
        # UI -         end
        # UI -     }
        # UI - }
    end

    def setup(world)
        @world = world
        # UI - @uimanager = UIManager.new("playing", @core)
        @jobmanager = JobManager.new(@world)
        @reticle = Reticle.new(world)
        # UI - @picker = Picker.new(@uimanager, @world)

        # UI - Event.add_listeners(@uimanager, @ap, @world, @reticle, @picker)
        Event.add_listeners(@ap, @world, @reticle, @picker)

        # Attach everything to the window before adding the UI stuff.
        # RENDER - @core.window.set_bg_color(0.2, 0.2, 0.2)
        # RENDER - view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        # RENDER - view.add_source(@world.active_camera.camera, 0)
        # RENDER - view.add_source(@uimanager, 1)

        # Add the actual UI elements.
        # UI - @console = @uimanager.create(Console, {:parent => @uimanager.root}) { |text| $logger.info "Eval-ing #{text}"; eval(text) }

        # UI - hud_tray = @uimanager.create(ElementContainer, {:parent=>@uimanager.root, :ldims=>[0,0,$lay_div,1], :snap=>[:bottom,:left], :grouping=>:row})
        # UI - hud_tray.add_element @uimanager.create(Button, {:text=>"Save World"}) {
        # UI -     @ap.call_action(:open_save_dialog)
        # UI - }
        # UI - hud_tray.add_element @uimanager.create(Button, {:text=>"Dwarves"}) {
        # UI -     @ap.call_action(:open_dwarves_menu)
        # UI - }
        # UI - hud_tray.add_element @uimanager.create(Button, {:text=>"Jobs"}) {
        # UI -     @ap.call_action(:open_job_menu)
        # UI - }
        # UI - hud_tray.add_element @uimanager.create(Button, {:text=>"Quit to Menu"}) {
        # UI -     @ap.call_action(:escape)
        # UI - }

        # DEBUG CODE
        # Add some test entities
        $logger.info "[+] Creating a test actor"
        @world.actors = []

        actor = create(Dwarf, "Franzibald")
        actor.set_position(0, 0, @world.get_surface(0,0)+1)
        actor = create(Dwarf, "Sheila")
        actor.set_position(0, 2, @world.get_surface(0,2)+1)

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
    
    def draw
        # @core.render_context.send(@wireframe ? :set_wireframe : :set_filled )
        @core.render_context.set_global_ambient(1.0, 1.0, 1.0, 1.0)
        @core.render_context.set_viewport(0, 0, @core.window.width, @core.window.height)
        @core.render_context.clear(1.0, 0.0, 0.0, 1.0)
        @world.render(@core.render_context)
    end

    def update(elapsed)
        # UI - @uimanager.update(elapsed)
        @world.update(elapsed)
    end

    def teardown
        # UI - Event.remove_listeners(@uimanager, @ap, @world, @reticle)
        Event.remove_listeners(@ap, @world, @reticle)
    end

    # Testing purposes only.
    def move_random
        begin
            x = rand(@world.width)
            y = rand(@world.height)
            z = @world.get_surface(x, y)+1
        end until !@world.out_of_bounds?(x, y, z)

        @jobmanager.add_job(Move, [x,y,z])
    end

    def mine_random
        begin
            x = rand(@world.width)
            y = rand(@world.height)
            z = @world.get_surface(x, y)
        end until !@world.out_of_bounds?(x, y, z)

        @jobmanager.add_job(Mine, [x,y,z])
    end
end

class Picker
    def initialize(uimanager, world)
        # UI - @uimanager = uimanager
        @world     = world
    end

    def make_selection(selected_group)
        unless @selection_list.nil?
            # UI - @uimanager.kill_element(@selection_list)
        end

        # Display information about the selected objects onscreen
        # UI - @selection_list = @uimanager.create(ElementContainer, {
        # UI -     :parent => @uimanager.root,
        # UI -     :snap=>[:right, :bottom],
        # UI -     :ldims=>[-2,2,10,10],
        # UI -     :grouping=>:column})
        # UI - selected_group.each do |actor|
        # UI -     @selection_list.add_element @uimanager.create(Pane, :text=>"#{actor.name}::#{actor.inspect}")
        # UI - end
    end

    def input_event(event)
        case event
        when MousePressed
            # UI - @start = [@uimanager.mouse.x, @uimanager.mouse.y]

        when MouseReleased
            # Can happen if the state starts with the mouse down.
            unless @start.nil?
                # UI - @end = [@uimanager.mouse.x, @uimanager.mouse.y]

                # Translate the coordinates into frustum space
                # UI - [@start, @end].each do |pair|
                # UI -     pair[0] = (pair[0] / @uimanager.width.to_f)
                # UI -     pair[1] = (pair[1] / @uimanager.height.to_f)
                # UI - end

                # Do picking
                # UI - selection = @world.pick_objects(@world.active_camera.camera, @start[0], @start[1], @end[0], @end[1])
                # UI - selected_group = []
                # UI - selection.each_actor { |actor| selected_group << actor }
                # UI - make_selection(selected_group)

                # UI - @start = nil
                # UI - @end   = nil
            end
        end
        return :unhandled
    end
end
