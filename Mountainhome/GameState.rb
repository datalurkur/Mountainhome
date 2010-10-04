require 'World'
require 'UIManager'
require 'EventTranslator'

class GameState < MHState
    include StateEventCreator

    def initialize(core)
        @core = core
    end

    def setup(world)
        @world = world
        @manager = UIManager.new("playing", @core)

        @evt = EventTranslator.new
        
        # Set some default actions; these have to be defined in GameState scope
        @evt.set_action(:toggle_console) { @console.toggle }
        @evt.set_action(:toggle_mouselook) { @mouselook = !@mouselook }
        @evt.set_action(:toggle_filled) { @wireframe = !@wireframe }
        @evt.set_action(:escape) { @core.set_state("MenuState") }
        @evt.set_action(:cycle_camera) {
            $logger.info "Switching active camera"
            new_cam = @world.cameras.first
            new_cam.set_active
            @world.cameras = @world.cameras[1...@world.cameras.size] << new_cam

            @core.window.clear_viewports
            view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
            view.add_source(@world.active_camera.camera, 0)
            view.add_source(@manager, 1)
        }
        @evt.set_action(:increase_depth) {
            @world.active_camera.change_depth(1) if @world.active_camera.respond_to?(:change_depth)
        }
        @evt.set_action(:decrease_depth) {
            @world.active_camera.change_depth(-1) if @world.active_camera.respond_to?(:change_depth)
        }

        # And some default events to trigger those actions. This will eventually
        # go away in favor of a GameOptions setter of some sort.
        @evt.set_event(:toggle_console,   Event.key_pressed(Keyboard.KEY_BACKQUOTE))
        @evt.set_event(:toggle_mouselook, Event.key_pressed(Keyboard.KEY_TAB))
        # Toggle between wireframe and filled when spacebar is pressed.
        @evt.set_event(:toggle_filled,    Event.key_pressed(Keyboard.KEY_SPACE))

        # Return to MenuState
        @evt.set_event(:escape,           Event.key_pressed(Keyboard.KEY_q))

        # Camera controls
        @evt.set_event(:cycle_camera,     Event.key_pressed(Keyboard.KEY_c))
        @evt.set_event(:increase_depth,   Event.key_pressed(Keyboard.KEY_PAGEDOWN))
        @evt.set_event(:decrease_depth,   Event.key_pressed(Keyboard.KEY_PAGEUP))

        # If the console is enabled, need to pass all keys to it FIRST.
        @evt.default_before_action do |event|
            if @console.toggled? && event[:state] == :pressed
                @console.input_event(event)
            end
        end

        # If there's no action defined for the event, pass it to manager and then world until it's handled.
        @evt.default_after_action do |event|
          status = @manager.input_event(event)
          @world.input_event(event) if status == :unhandled
        end


        # Attach everything to the window before adding the UI stuff.
        @core.window.set_bg_color(0.2, 0.2, 0.2)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.active_camera.camera, 0)
        view.add_source(@manager, 1)

        # Add the actual UI elements.
        @console = @manager.create(Console, {:parent => @manager.root}) { |text| $logger.info "Eval-ing #{text}"; eval(text) }

        @manager.create(Button, {:parent=>@manager.root, :text=>"Save World", :snap=>[:left,:bottom], :ldims=>[1,1,4,1]}) {
            @manager.create(InputDialog, {:parent=>@manager.root, :text=>"Save world as..."}) { |filename|
                if filename.length > 0
                    @world.save(@core.personal_directory + filename)
                    :accept
                else
                    @manager.create(InfoDialog, {:parent=>@manager.root, :text=>"Please enter a filename."})
                    :reject
                end
            }
        }

        # DEBUG CODE
        # Add some test entities
        $logger.info "[+] Creating a test actor"
        @world.actors = []
        # Dwarves currently have a radius of 0.4, so offset the zposition by this much.
        @world.create_actor(Dwarf, "Franzibald", "Dwarf", "Dwarf", [0, 0, @world.terrain.get_surface(0,0) + 0.4])

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
        @manager.update(elapsed)
        @world.update(elapsed)
    end

    def teardown
        @core.window.clear_viewports
    end

    def key_pressed(event)
        @evt.translate(event)
    end

    def key_released(event)
        @evt.translate(event)
    end

    def mouse_pressed(event)
        @evt.translate(event)
    end

    def mouse_released(event)
        @evt.translate(event)
    end

    def mouse_moved(event)
        (@mouselook ? @world : @manager).input_event(event)
    end
end
