class GameState < State
    def setup
        puts "SETTING UP"
    end

    def update(elapsed)
        puts "UPDATING: #{elapsed}"
    end

    def teardown
        puts "TEARING DOWN"
    end
end

$mountainhome.register_state(GameState.new, "RubyGameState")
$mountainhome.state = "RubyGameState"
