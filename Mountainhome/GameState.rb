require 'WorldFactory'
require 'UIManager'

class GameState < MHState
    def initialize(core)
        @core = core
        super()
    end

    def setup
        $logger.info "Generating the world."
        @world = WorldFactory.generateWorld(@core, 7)
        @world.populate()

        $logger.info "Initializing the UI."
        # Specify console evaluation code
        @manager = UIManager.new("default", @core)

        @core.window.set_bg_color(0.4, 0.6, 0.8)
        view = @core.window.add_viewport(0, 0.0, 0.0, 1.0, 1.0)
        view.add_source(@world.camera, 0)
        view.add_source(@manager, 1)

        evaluator = Proc.new do |cmd| 
            begin
                eval(cmd)
            rescue StandardError, SyntaxError, NameError => err_msg
                err_msg
            rescue
                "Invalid console input"
            end
        end

        # Set up the root element
        @console = Console.new(@manager, evaluator)
    end

    def update(elapsed)
        # GC.start
        @manager.update(elapsed)
        @world.update(elapsed)
    end

    def teardown
        # Do nothing!
    end
    
    def key_typed(key, modifier)
        $logger.info("key_typed:    #{key.to_s} + #{modifier.to_s}")
    end

    def key_pressed(key, modifier)
        args = {:type => :keyboard, :state => :pressed, :key => key, :modifier => modifier}
        status = @manager.input_event(args)
        status = @world.input_event(args) if status == :unhandled
        case key
        when Keyboard.KEY_BACKQUOTE
            if args[:state] == :pressed
                @active_element = (@active_element ? nil : @console)
                @console.toggle
            end
        when Keyboard.KEY_q
            $logger.info "Received quit keypress"
            self.teardown
        end if status == :unhandled
        $logger.info("key_pressed:  #{key.to_s} + #{modifier.to_s}") if status == :unhandled
    end

    def key_released(key, modifier)
        args = {:type => :keyboard, :state => :released, :key => key, :modifier => modifier}
        status = @manager.input_event(args)
        status = @world.input_event(args) if status == :unhandled
        $logger.info("key_released: #{key.to_s} + #{modifier.to_s}") if status == :unhandled
    end

    def mouse_moved(absX, absY, relX, relY)
        args = {:type => :move, :abs_x => absX, :abs_y => absY, :x => relX, :y => relY}
		status = @manager.input_event(args)
        status = @world.input_event(args) if status == :unhandled
        $logger.info("mouse_moved: #{[absX, absY, relX, relY].inspect}") if status == :unhandled
    end

    def mouse_clicked(button, x, y)
        $logger.info("mouse_clicked:  #{button.to_s} + #{x.to_s} + #{y.to_s}")
    end

    def mouse_pressed(button, x, y)
        args = {:type => :mouse, :button => button, :state => :pressed, :x => x, :y => y}
        status = @manager.input_event(args)
        $logger.info("mouse_pressed:  #{button.to_s} + #{x.to_s} + #{y.to_s}") if status == :unhandled
    end

    def mouse_released(button, x, y)
        args = {:type => :mouse, :button => button, :state => :released, :x => x, :y => y}
        status = @manager.input_event(args)
        $logger.info("mouse_released: #{button.to_s} + #{x.to_s} + #{y.to_s}") if status == :unhandled
    end
end
