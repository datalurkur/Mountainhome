require 'WorldFactory.rb'

class GameState < State
    def setup
		puts "SETTING UP"
	    fact = WorldFactory.new
		world = fact.generateWorld(4,4)
		world.populate
    end

    def update(elapsed)
        #puts "UPDATING: #{elapsed}"
    end

    def teardown
        puts "TEARING DOWN"
    end
end

$mountainhome.register_state(GameState.new, "RubyGameState")
$mountainhome.state = "RubyGameState"
