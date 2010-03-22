require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
		super(args[:looknfeel])

        @root = new_element(:name => "root", :mat => "", :text => "", :x => 0, :y => 0, :w => 800, :h => 600)
        set_root(@root)

        @mouse = new_element(:name => "mouse", :mat => "cursor", :text => "", :x => 0, :y => 0, :w => 14, :h => 21)
        @root.add_child(@mouse)

        @active = true
    end

    def teardown
        $logger.info "Tearing down UIManager"
    end

    def input_event(args={})
        case args[:type]
        when :mouse
			case args[:state]
			when :pressed
                #name = "element"
                #@root.add_child  new_element(:name => name, :mat => "white", :text => name,
                #            :x => args[:x], :y => args[:y],
                #            :w => 100, :h => 20)
			when :released
			end
			return :handled
		when :move
            return :unhandled if (not @active)

            # FIXME - update with actual dimensions
			@mouse.x = args[:abs_x] #[[@mouse.x + args[:x], 0].max, 800].min
			@mouse.y = args[:abs_y] #[[@mouse.y - args[:y], 0].max, 600].min
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
        uie = UIElement.new(args[:name], self, args[:mat], args[:text])
        uie.set_dimensions(args[:x],args[:y],args[:w],args[:h])
        return uie
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
