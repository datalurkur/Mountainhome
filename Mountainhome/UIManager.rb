require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
		super(args[:looknfeel])
        @elements = []
		@mouse = UIElement.new("mouse", self, "cursor", "")
		@mouse.set_dimensions(0.0, 0.0, 0.015, 0.03)
        @active = true
    end

    def teardown
        $logger.info "Tearing down UIManager"
    end

    def input_event(args={})
        case args[:type]
        when :mouse
            $logger.info "UIManager received a mouseclick with args #{args.inspect}"
			case args[:state]
			when :pressed
				$logger.info "Click..."
			when :released
				$logger.info "...Released"
			end
			return :handled
		when :move
            return :unhandled if (not @active)

            # FIXME - update with actual dimensions
			@mouse.x = [[@mouse.x + (args[:x] / 800.0), 0].max, 1].min
			@mouse.y = [[@mouse.y - (args[:y] / 600.0), 0].max, 1].min
			return :handled
        when :keyboard
            return :unhandled if (args[:state] == :released)
            
            case args[:key]
            when Keyboard.KEY_SPACE
                @active = (not @active)
                $logger.info "Setting UIManager activity to #{@active}"
                return :handled
            when Keyboard.KEY_UP, Keyboard.KEY_DOWN, Keyboard.KEY_LEFT, Keyboard.KEY_RIGHT
                return :unhandled if (not @active)
				
                $logger.info "UIManager receives keypad input #{args[:key]}"
                return :handled 
            else
                $logger.info "UIManager deferred handling of #{args[:key]} to GameState"
                return :unhandled
            end
        else
            return :unhandled
        end
    end

    def new_element(args={})
		$logger.info "Adding a new element to UIManager with args #{args.inspect}"
        uie = UIElement.new(args[:name], self, "white")
        uie.set_dimensions(args[:x],args[:y],args[:w],args[:h])
		@elements << uie
    end

    # Find the topmost menu element at [x,y]
    def element_at(x, y)
        # Find all elements that exist at [x,y], keeping track of the one with the greatest depth from the root element
        topmost = nil
        @elements.each do |element|
            if (x >= element.x) and (x <= (element.x+element.width)) and
               (y >= element.y) and (y <= (element.y+element.height))
                (topmost = element) if ( (topmost == nil) or (element.depth > topmost.depth) )
            end
        end
        return topmost
    end
end
