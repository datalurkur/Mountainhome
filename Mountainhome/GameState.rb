require 'WorldFactory'

class GameState < MHGameState
    def setup
        $logger.info "Generating the world."
        
        $logger.info(Keyboard.KEY_a.to_s)
        
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
end
