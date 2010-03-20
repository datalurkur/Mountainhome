require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
		super(args[:looknfeel])
        @elements = []
		@mouse = UIElement.new("mouse", self, "", "M")
		@mouse.set_dimensions(0.0, 0.0, 0.025, 0.05)
		@mouse_pos = [0.0, 0.0]
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
			when :down
				$logger.info "Click..."
			when :up
				$logger.info "...Released"
			end
			return :handled
		when :move
            return :unhandled if (not @active)

            # FIXME - update with actual dimensions
            width=800.0
            height=600.0
			@mouse_pos[0] += args[:x]/width
			@mouse_pos[1] -= args[:y]/height
			@mouse_pos.each_with_index do |dim,index|
				@mouse_pos[index] = 0 if dim < 0
				@mouse_pos[index] = 1 if dim > 1
			end
			@mouse.set_dimensions(@mouse_pos[0], @mouse_pos[1], 0.025, 0.05)

			return :handled
        when :keyboard
            return :unhandled if (args[:state] == :up)
            
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
