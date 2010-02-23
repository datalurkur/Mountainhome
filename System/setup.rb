require 'WorldFactory.rb'

class GameState < State
    def setup
        $logger.info "SETTING UP"
		fact = WorldFactory.new
		world = fact.generateWorld(4,4)
		#TODO - figure out why different tile types don't properly inherit Instantiable
		world.populate
    end

    def update(elapsed)
        $logger.info "UPDATING: #{elapsed}"
    end

    def teardown
        $logger.info "TEARING DOWN"
    end
end

$mountainhome.register_state(GameState.new, "RubyGameState")
$mountainhome.state = "RubyGameState"
