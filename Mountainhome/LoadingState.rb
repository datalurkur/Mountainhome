require 'UIManager'
require 'TopMenuBuilder'

require 'World'
require 'TerrainBuilder'

class LoadingState < MHLoadingState
    def setup
        # Specify console evaluation code
        evaluator = Proc.new do |cmd| 
            begin
                eval(cmd)
            rescue StandardError, SyntaxError, NameError => err_msg
                err_msg
            rescue
                "Invalid console input"
            end
        end

        self.manager = UIManager.new(:looknfeel => "default", :eval_proc => evaluator)

        TopMenuBuilder.create_top_menus(manager, self)

        # Set up the gamestate
    end

    def create_world
        # Clear the main menu
        manager.clear_elements

        # Create the world
        world = World.new(:height => 129, :width => 129, :depth => 65)

        # Create the gamestate
        game_state = GameState.new
        game_state.world = world
        game_state.manager = self.manager

        # Switch to the new state
        $mhcore.register_state(game_state, "GameState")
        $mhcore.state = "GameState"
    end

    def update(elapsed)
        self.manager.update(elapsed)
    end

    def teardown
        super
    end
    
    def mouse_moved(absX, absY, relX, relY)
        args = {:type => :move, :abs_x => absX, :abs_y => absY, :x => relX, :y => relY}
		status = self.manager.input_event(args)
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
