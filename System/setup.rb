require 'WorldFactory.rb'

class GameState # GameState is already partialy defined in C++.
    def setup
        $logger.info "SETTING UP"
        @world = WorldFactory.generateWorld(5,5)
		@world.populate(self.scene)
    end

    def update(elapsed)
    #    $logger.info "UPDATING: #{elapsed}"
    end

    def teardown
        $logger.info "TEARING DOWN"
    end
end

$mountainhome.register_state(GameState.new, "RubyGameState")
$mountainhome.state = "RubyGameState"
