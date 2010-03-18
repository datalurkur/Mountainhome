require 'WorldFactory'

class GameState < MHGameState
    def setup
        $logger.info "Generating the world."
        self.world = WorldFactory.generateWorld(7)
		self.world.populate()
    end

    def update(elapsed)
    #    $logger.info "UPDATING: #{elapsed}"
    end

    def teardown
        $logger.info "Destroying the world."
        self.world = nil
    end
    
    def key_typed(key, modifier)
        $logger.info("key_typed:    #{key.to_s} + #{modifier.to_s}")
    end

    def key_pressed(key, modifier)
        $logger.info("key_pressed:  #{key.to_s} + #{modifier.to_s}")
    end

    def key_released(key, modifier)
        $logger.info("key_released: #{key.to_s} + #{modifier.to_s}")
    end

    def mouse_moved(absX, absY, relX, relY)
        $logger.info("mouse_moved:    #{absX.to_s} + #{absY.to_s} #{relX.to_s} + #{relY.to_s}")
    end

    def mouse_clicked(button, x, y)
        $logger.info("mouse_clicked:  #{button.to_s} + #{x.to_s} + #{y.to_s}")
    end

    def mouse_pressed(button, x, y)
        $logger.info("mouse_pressed:  #{button.to_s} + #{x.to_s} + #{y.to_s}")
    end

    def mouse_released(button, x, y)
        $logger.info("mouse_released: #{button.to_s} + #{x.to_s} + #{y.to_s}")
    end
end
