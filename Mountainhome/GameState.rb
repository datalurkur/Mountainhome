require 'WorldFactory'
require 'UIManager'

class GameState < MHGameState
    def setup
        $logger.info "Generating the world."
        self.world = WorldFactory.generateWorld(7)
        self.world.populate()

        $logger.info "Initializing the UI."
        @manager = UIManager.new(:looknfeel => "default") # Later, this will be passed a looknfeel
        
        @manager.new_element(:name => "freeblspooge", :x => 0, :y => 0, :z => 10,
                                                      :w => 5,   :h => 5,   :d => 0)
    end

    def update(elapsed)
    #    $logger.info "UPDATING: #{elapsed}"
    end

    def teardown
        $logger.info "Destroying the world."
        self.world = nil

        $logger.info "Destroying the UI."
        @manager.teardown
        @manager = nil
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
        status = @manager.input_event(:type => :mouse, :button => button, :state => :down, :x => x, :y => y)
        status = nil
        if status == :unhandled
            $logger.info("mouse_pressed:  #{button.to_s} + #{x.to_s} + #{y.to_s}")
        end
    end

    def mouse_released(button, x, y)
        status = @manager.input_event(:type => :mouse, :button => button, :state => :up, :x => x, :y => y)
        if status == :unhandled
            $logger.info("mouse_released: #{button.to_s} + #{x.to_s} + #{y.to_s}")
        end
    end
end
