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
end
