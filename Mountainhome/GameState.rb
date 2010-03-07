require 'WorldFactory'

class RubyGameState < GameState
    def setup
        $logger.info "SETTING UP"
        self.world = WorldFactory.generateWorld(5, 5)
		self.world.populate()
    end

    def update(elapsed)
    #    $logger.info "UPDATING: #{elapsed}"
    end

    def teardown
        $logger.info "TEARING DOWN"
        self.world = nil
    end
end
