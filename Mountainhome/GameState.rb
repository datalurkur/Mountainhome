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

        # Frustum culling is on by default.
        @frustum_culling = true
    end

    def initialize_actions
        # Create a new pack with keybindings registered in GameStateAP.rb.
        @ap = ActionPack.new("GameStateAP")

        # Set some default actions; these have to be defined in GameState scope
        @ap.register_action(:toggle_console) {
            (@console.visible) ? @console.teardown : @console.setup
        }

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

        @ap.register_action(:toggle_path_visualizer) {
            @path_visualizer = !@path_visualizer
            @world.draw_path_visualizer = @path_visualizer
        }

        @ap.register_action(:toggle_paused) {
            self.toggle_paused
        }

        @ap.register_action(:quit_to_menu) {
            @core.set_state("MenuState")
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

        # XXXBMW Swapped these to get zoom in/out working in favor over free move up/down.
        # Need to consider  how to handle the differences between these two. It seems most
        # likely we'll need to make all of this modal, rather than having one giant AP for everything.
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
        @ap.register_action(:open_save_dialog) {
            $logger.info "Save dialogue not yet implemented."
        }

        @ap.register_action(:open_right_click_menu) { |x,y|
            unless @right_click_menu.nil?
                @uimanager.delete_child(@right_click_menu)
            end

            @right_click_menu = @uimanager.create(ContextMenu, {:x => x, :y => y, :lay_dims=>[4,1], :values => ["Mine", "Move", "Select Item", "Move Selected Item To", "Build Wall"]}) { |val|
                $logger.info "#{val} selected"
                case val
                when "Mine"
                    @picker.selected_voxels.each do |position|
                        if @world.get_voxel_parameter(*position, :to_mine) == false
                            @jobmanager.add_job(Mine, position)
                            @world.set_voxel_parameter(*position, :to_mine, true)
                        end
                    end
                when "Move"
                    @picker.selected_voxels.each do |position|
                        position[2] += 1
                        @jobmanager.add_job(Move, position)
                    end
                when "Select Item"
                    # look above first selected voxel
                    position = @picker.selected_voxels.first
                    position[2] += 1
                    actors = @world.find(Item, {:position => position})
                    @selected_object = actors.first if actors && !actors.empty?
                    $logger.info "selected_object is #{@selected_object.inspect}"
                when "Move Selected Item To"
                    if @selected_object
                        to_position = @picker.selected_voxels.first
                        to_position[2] += 1
                        $logger.info "to_position is #{to_position.inspect}"
                        @jobmanager.add_job(MoveObject, to_position, @selected_object)
                        @selected_object = nil
                    end
                when "Build Wall"
                    @picker.selected_voxels.each do |voxel|
                        voxel[2] += 1
                        @jobmanager.add_job(BuildWall, voxel)
                    end
                end
            }
        }
    end

    ###
    # State management code
    ###

    def setup(world, args={})
        @world = world
        @paused = args[:start_paused]

        @uimanager = UIManager.new(@core.window.width, @core.window.height)
        @jobmanager = JobManager.new(@world)

        # Set the default mouselook/cursor values.
        @uimanager.cursor_enabled = true
        @mouselook = false

        Event.add_listeners(@ap, self)

        @picker = Picker.new(@uimanager, @world)

        initialize_ui

        # Add the actual UI elements.
        @console = @uimanager.create(Console, {:lay_pos => [0,$max_dim/2], :lay_dims =>[$max_dim,$max_dim/2]}) do |text|
            eval(text)
        end

        @uimanager.create(Button, {:lay_pos=>[0,$max_dim-1], :lay_dims=>[$max_dim*0.25,1], :text=>"Save World"}) {
            @ap.call_action(:open_save_dialog)
        }
        @uimanager.create(Button, {:lay_pos=>[$max_dim*0.75,$max_dim-1], :lay_dims=>[$max_dim*0.25,1], :text=>"Quit to Menu"}) {
            @ap.call_action(:quit_to_menu)
        }

        # DEBUG CODE
        # Add some test entities
        $logger.info "[+] Creating a test actor"
        @world.actors = []

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

        args[:on_launch].call(self) unless args[:on_launch].nil?
    end

    # Actor and MHEntity creation.
    def create(klass, name)
        actor = @world.create(klass, name)
        # Tell the job manager about the new Worker it needs to maintain.
        if klass.parent_module.extensions.include?(WorkerExtension)
            @jobmanager.add_worker(actor)
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
        unless @paused
            @world.update(elapsed)
            @jobmanager.update(elapsed)
        end

        @uimanager.update(elapsed)

        #update the active camera
        sensitivity = 1.0
        @world.active_camera.adjust_pitch(@pitch * sensitivity)
        @world.active_camera.adjust_yaw(  @yaw   * sensitivity)
        @pitch = @yaw = 0

        move = @movement.collect { |elem| elem * elapsed }
        @world.active_camera.move_relative(*move)
    end

    def teardown
        Event.clear_listeners

        # Clear things out to force GC to clean everything up.
        @uimanager = nil
        @jobmanager = nil
        @picker = nil

        # XXXBMW: There is a ruby bug where the world is never destroyed because we use
        # it with fibers. Because of this, we force destruction manually.
        @world.destroy_instance
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
        when MousePressed
            if event.button == MOUSEBUTTON_RIGHT
                @ap.call_action(:open_right_click_menu, event.x, event.y)
                return :handled
            end
        end
        return :unhandled
    end

    ###
    # For executing tests
    ###

    def run(test_name)
        begin
            $logger.info "Executing #{test_name}"
            file_data = File.read("Tests/#{test_name}.rb")
            eval(file_data)
        rescue
            $logger.info Dir.pwd
            $logger.info "Failed to execute test #{test_name} - #{$!}"
        end
    end

    def toggle_paused
        @paused = !@paused
        $logger.info "Mountainhome is now #{@paused ? "" : "un"}paused."
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
            z = @world.get_surface_level(x, y) + 1
        end until !@world.out_of_bounds?(x, y, z)

        @jobmanager.add_job(Move, [x,y,z])
    end

    def mine_random
        begin
            x = rand(@world.width)
            y = rand(@world.height)
            z = @world.get_surface_level(x, y)
        end until !@world.out_of_bounds?(x, y, z)

        @jobmanager.add_job(Mine, [x,y,z])
    end

    def shitty_create_actor(to_create)
        to_create.each do |params|
            actor = create(*params)
            # Try 100 times to find a place for this actor to stand
            100.times do
                random_position = [rand(@world.width), rand(@world.height)]
                random_position << @world.get_surface_level(random_position[0], random_position[1]) + 1
                # FIXME - This really should be more intelligent - need to check whether there are other objects blocking this space, etc
                if @world.solid_ground?(random_position[0], random_position[1], random_position[2] - 1)
                    actor.set_position(*random_position)
                    break
                else
                    next
                end
            end
        end
    end
end

class Picker
    attr_writer :selected_voxels,
                :selected_actors

    def initialize(uimanager, world)
        @uimanager = uimanager
        @world     = world

        Event.add_listener(self)
    end

    # Query our selection for information.
    def selected_voxels
        selected_voxels = []
        return selected_voxels if @selection.nil?
        @selection.each_voxel do |voxel|
                selected_voxels << voxel
        end
        selected_voxels
    end

    def selected_actors
        selected_actors = []
        return selected_actors if @selection.nil?
        @selection.each_actor do |actor|
            selected_actors << actor
        end
        selected_actors
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

                # Deselect previously selected things
                self.selected_voxels.each do |voxel|
                    @world.deselect_voxel(voxel[0], voxel[1], voxel[2])
                end

                self.selected_actors.each do |actor|
                    # Add actor deselection code when it's necessary
                end

                # Do picking
                @selection = @world.pick_objects(@world.active_camera, @start[0], @start[1], @end[0], @end[1])

                # Select newly selected things
                if @selection.num_actors > 0
                    $logger.info "Selected #{@selection.num_actors} actors"
                end
                if @selection.num_voxels > 0
                    unselect = []
                    @selection.each_voxel do |voxel|
                        voxel_type = @world.get_voxel_type(*voxel)
                        if voxel_type && voxel_type.has_parameter?(:selected)
                            @world.select_voxel(*voxel)
                        else
                            unselect << voxel
                        end
                    end
                    unselect.each { |voxel| @selection.remove_voxel(*voxel) }
                    $logger.info "Selected #{@selection.num_voxels} voxels"
                end

                @start = nil
                @end   = nil
            end
        end
        return :unhandled
    end
end
