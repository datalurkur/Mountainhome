require 'World'
require 'UIManager'
require 'ActionPack'
require 'Camera'

class GameState < MHState
    ###
    # Initialization methods.
    ###

    def initialize(core)
        @core = core
        initialize_actions
        initialize_ui

        # Frustum culling is on by default.
        @frustum_culling = true
    end

    def initialize_actions
            # Create a new pack with keybindings registered in GameStateAP.rb.
        @ap = ActionPack.new("GameStateAP")

        # Set some default actions; these have to be defined in GameState scope
        @ap.register_action(:toggle_console) { @console.toggle }

        @ap.register_action(:toggle_mouselook) do
            self.toggle_mouselook unless td_camera_is_active
        end

        @ap.register_action(:toggle_filled) { @wireframe = !@wireframe }

        @ap.register_action(:toggle_frustum_culling) {
            @frustum_culling = !@frustum_culling
            @world.frustum_culling = @frustum_culling
        }

        @ap.register_action(:toggle_bounding_boxes) {
            @bounding_boxes = !@bounding_boxes
            @world.draw_bounding_boxes = @bounding_boxes
        }

        @ap.register_action(:quit_to_menu) {
            @core.set_state("MenuState")
            @world = nil
        }
        @ap.register_action(:cycle_camera) {
            @world.cycle_cameras

            # Make sure we enable/disable the mouse pointer here.
            @uimanager.cursor_enabled = td_camera_is_active ? true : !@mouselook
        }

        @ap.register_action(:increase_depth) {
            @world.active_camera.change_depth(1) if @world.active_camera.respond_to?(:change_depth)
        }
        @ap.register_action(:decrease_depth) {
            @world.active_camera.change_depth(-1) if @world.active_camera.respond_to?(:change_depth)
        }

        # XXXBMW: Shift interaction here is weird. Pressing it or letting go at the proper time will result in move lock.

        movement_speed = 0.01
        @ap.register_action(:move_right)    { @movement[0] =  movement_speed }
        @ap.register_action(:move_left)     { @movement[0] = -movement_speed }
        @ap.register_action(:move_forward)  { @movement[1] =  movement_speed }
        @ap.register_action(:move_backward) { @movement[1] = -movement_speed }

        # XXXBMW Swapped these to get zoom in/out working in favor over free move up/down. Need to consider  how to handle the differences between these two. It seems most likely we'll need to make all of this modal, rather than having one giant AP for everything.
        @ap.register_action(:move_down)     { @movement[2] =  movement_speed }
        @ap.register_action(:move_up)       { @movement[2] = -movement_speed }

        @ap.register_action(:stop_right)    { @movement[0] = 0 if @movement[0] > 0 }
        @ap.register_action(:stop_left)     { @movement[0] = 0 if @movement[0] < 0 }
        @ap.register_action(:stop_forward)  { @movement[1] = 0 if @movement[1] > 0 }
        @ap.register_action(:stop_backward) { @movement[1] = 0 if @movement[1] < 0 }
        @ap.register_action(:stop_down)     { @movement[2] = 0 if @movement[2] > 0 }
        @ap.register_action(:stop_up)       { @movement[2] = 0 if @movement[2] < 0 }
    end

    def initialize_ui
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

    ###
    # State management code
    ###

    def setup(world)
        @world = world

        @uimanager = UIManager.new(@core.window.width, @core.window.height)
        @jobmanager = JobManager.new(@world)
        @reticle = Reticle.new(world)

        # Set the default mouselook/cursor values.
        @uimanager.cursor_enabled = true
        @mouselook = false

        @picker = Picker.new(@uimanager, @world)

        Event.add_listeners(@uimanager, @ap, @world, @reticle, @picker, self)

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
        actor.set_position(0, 0, @world.get_surface(0,0) + 1)
        actor = create(Dwarf, "Sheila")
        actor.set_position(0, 2, @world.get_surface(0,2) + 1)

        # Invoke the managers
        $logger.info "[+] Invoking managers' seed methods"
        MountainhomeDSL.managers.each_value do |manager|
            manager.clear
            manager.seed(@world) if manager.respond_to? :seed
        end

        # Define some initial values.
        @mouselook = false
        @pitch = 0
        @yaw = 0
        @movement = [0, 0, 0]

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
        @core.render_context.set_wireframe(@wireframe)
        @core.render_context.set_viewport(0, 0, @core.window.width, @core.window.height)
        @core.render_context.clear(0.39, 0.58, 0.93, 1.0)
        @world.render(@core.render_context)
        @uimanager.render(@core.render_context)
    end

    def update(elapsed)
        @uimanager.update(elapsed)
        @world.update(elapsed)

        #update the active camera
        sensitivity = 1.0
        @world.active_camera.adjust_pitch(@pitch * sensitivity)
        @world.active_camera.adjust_yaw(  @yaw   * sensitivity)
        @pitch = @yaw = 0

        move = @movement.collect {|elem| elem * elapsed}
        @world.active_camera.move_relative(*move)
    end

    def teardown
        Event.remove_listeners(@uimanager, @ap, @world, @reticle, @picker, self)

        # Set the world to nil so it gets GC'ed.
        @world = nil
    end

    def input_event(event)
        case event
        when MouseMoved
            if @mouselook && !td_camera_is_active
                rotate_speed = -0.002
                @yaw   = event.relX * rotate_speed
                @pitch = event.relY * rotate_speed
                return :handled
            end
        end
        return :unhandled
    end

    ###
    # Other methods
    ###
    def fp_camera_is_active; @world.active_camera.class == FirstPersonCamera; end
    def td_camera_is_active; @world.active_camera.class == TopCamera;         end

    def toggle_mouselook
        @mouselook = !@mouselook
        @uimanager.toggle_cursor
    end

    def move_random
        begin
            x = rand(@world.width)
            y = rand(@world.height)
            z = @world.get_surface(x, y) + 1
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
        @uimanager = uimanager
        @world     = world

        @selected_tiles = []
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
                    pair[0] = (pair[0] / @uimanager.w.to_f)
                    pair[1] = (pair[1] / @uimanager.h.to_f)
                end

                # Do picking
                selection = @world.pick_objects(@world.active_camera, @start[0], @start[1], @end[0], @end[1])

                @selected_tiles.each do |tile|
                    @world.deselect_tile(tile[0], tile[1], tile[2])
                end
                @selected_tiles = []

                if selection.num_actors > 0
                    $logger.info "Selected #{selection.num_actors} actors"
                    selection.each_actor do |actor|
                        $logger.info "[+] #{actor.respond_to?(:name) ? actor.name + " the " : ""}#{actor.class} selected"
                    end
                elsif selection.num_tiles > 0
                    $logger.info "Selected #{selection.num_tiles} tiles"
                    selection.each_tile do |tile|
                        @world.select_tile(tile[0], tile[1], tile[2])
                        @selected_tiles << tile
                    end
                end

                @start = nil
                @end   = nil
            end
        end
        return :unhandled
    end
end
