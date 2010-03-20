require 'WorldFactory'
require 'UIManager'

class GameState < MHGameState
    def setup
        $logger.info "Generating the world."
        self.world = WorldFactory.generateWorld(7)
        self.world.populate()

        $logger.info "Initializing the UI."
        self.manager = UIManager.new(:looknfeel => "default") # Later, this will be passed a looknfeel
    end

    def update(elapsed)
        self.world.update(elapsed)
    end

    def teardown
        $logger.info "Destroying the world."
        self.world = nil

        $logger.info "Destroying the UI."
        self.manager.teardown
        self.manager = nil
    end
    
    def key_typed(key, modifier)
        $logger.info("key_typed:    #{key.to_s} + #{modifier.to_s}")
    end

    def key_pressed(key, modifier)
        args = {:type => :keyboard, :state => :down, :key => key, :modifier => modifier}
        status = self.manager.input_event(args)
        status = self.world.input_event(args) if status == :unhandled
        $logger.info("key_pressed:  #{key.to_s} + #{modifier.to_s}") if status == :unhandled
    end

    def key_released(key, modifier)
        args = {:type => :keyboard, :state => :up, :key => key, :modifier => modifier}
        status = self.manager.input_event(args)
        status = self.world.input_event(args) if status == :unhandled
        $logger.info("key_released: #{key.to_s} + #{modifier.to_s}") if status == :unhandled
    end

    def mouse_moved(absX, absY, relX, relY)
        args = {:type => :move, :x => relX, :y => relY}
		status = self.manager.input_event(args)
        status = self.world.input_event(args) if status == :unhandled
        $logger.info("mouse_moved: #{[absX, absY, relX, relY].inspect}") if status == :unhandled
    end

    def mouse_clicked(button, x, y)
        $logger.info("mouse_clicked:  #{button.to_s} + #{x.to_s} + #{y.to_s}")
    end

    def mouse_pressed(button, x, y)
        status = self.manager.input_event(:type => :mouse, :button => button, :state => :down, :x => x, :y => y)
        $logger.info("mouse_pressed:  #{button.to_s} + #{x.to_s} + #{y.to_s}") if status == :unhandled
    end

    def mouse_released(button, x, y)
        status = self.manager.input_event(:type => :mouse, :button => button, :state => :up, :x => x, :y => y)
        $logger.info("mouse_released: #{button.to_s} + #{x.to_s} + #{y.to_s}") if status == :unhandled
    end
end
