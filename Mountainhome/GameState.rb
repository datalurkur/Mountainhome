require 'WorldFactory'
require 'UIManager'

class GameState < MHGameState
    def setup
        $logger.info "Generating the world."
        self.world = WorldFactory.generateWorld(7)
        self.world.populate()

        $logger.info "Initializing the UI."
        # Specify console evaluation code
        evaluator = Proc.new do |cmd| 
            begin
                eval(cmd)
            rescue
                "Invalid console input"
            end
        end
        self.manager = UIManager.new(:looknfeel => "default", :eval_proc => evaluator) 
    end

    def update(elapsed)
        self.manager.update(elapsed)
        self.world.update(elapsed)
    end

    def teardown
        super
    end
    
    def key_typed(key, modifier)
        $logger.info("key_typed:    #{key.to_s} + #{modifier.to_s}")
    end

    def key_pressed(key, modifier)
        args = {:type => :keyboard, :state => :pressed, :key => key, :modifier => modifier}
        status = self.manager.input_event(args)
        status = self.world.input_event(args) if status == :unhandled
        if (status == :unhandled and key == Keyboard.KEY_q)
            $logger.info "Received quit keypress"
            self.teardown
        end
        $logger.info("key_pressed:  #{key.to_s} + #{modifier.to_s}") if status == :unhandled
    end

    def key_released(key, modifier)
        args = {:type => :keyboard, :state => :released, :key => key, :modifier => modifier}
        status = self.manager.input_event(args)
        status = self.world.input_event(args) if status == :unhandled
        $logger.info("key_released: #{key.to_s} + #{modifier.to_s}") if status == :unhandled
    end

    def mouse_moved(absX, absY, relX, relY)
        args = {:type => :move, :abs_x => absX, :abs_y => absY, :x => relX, :y => relY}
		status = self.manager.input_event(args)
        status = self.world.input_event(args) if status == :unhandled
        $logger.info("mouse_moved: #{[absX, absY, relX, relY].inspect}") if status == :unhandled
    end

    def mouse_clicked(button, x, y)
        $logger.info("mouse_clicked:  #{button.to_s} + #{x.to_s} + #{y.to_s}")
    end

    def mouse_pressed(button, x, y)
        args = {:type => :mouse, :button => button, :state => :pressed, :x => x, :y => y}
        status = self.manager.input_event(args)
        $logger.info("mouse_pressed:  #{button.to_s} + #{x.to_s} + #{y.to_s}") if status == :unhandled
    end

    def mouse_released(button, x, y)
        args = {:type => :mouse, :button => button, :state => :released, :x => x, :y => y}
        status = self.manager.input_event(args)
        $logger.info("mouse_released: #{button.to_s} + #{x.to_s} + #{y.to_s}") if status == :unhandled
    end
end
