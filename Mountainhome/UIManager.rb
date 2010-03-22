require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
		super(args[:looknfeel])

        @root = new_element(:name => "root", :mat => "", :text => "", :x => 0, :y => 0, :w => 800, :h => 600)
        set_root(@root)

        @mouse = new_element(:name => "mouse", :mat => "cursor", :text => "", :x => 0, :y => 0, :w => 14, :h => 21)
        @mouse.set_offset(0, -21)
        @mouse.always_on_top
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
                hit = element_at(@mouse.x, @mouse.y)
                if hit
                    kill_element(hit)
                else
                    name = "element"
                    elem = new_element(:name => name, :mat => "white", :text => name,
                                       :x => @mouse.x, :y => @mouse.y,
                                       :w => 100, :h => 20, :clickable => true)
                    @root.add_child(elem)
                end
			when :released
			end
			return :handled
		when :move
            return :unhandled if (not @active)

            # FIXME - update with actual dimensions
			@mouse.x = [[@mouse.x + args[:x], 0].max, 800].min
			@mouse.y = [[@mouse.y - args[:y], 0].max, 600].min
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
        args[:clickable] ? add = {:clickable => true} : add = {}
        uie = UIElement.new(args[:name], self, args[:mat], args[:text], add)
        uie.set_dimensions(args[:x],args[:y],args[:w],args[:h])
        return uie
    end

    def kill_element(elem)
        @root.cull_child(elem)
    end

    # Find the topmost menu element at [x,y]
    def element_at(x, y)
        elems = @root.elements_at(x,y,0)
        topmost = {:element => nil, :d => -1}
        elems.each do |element|
            topmost = element if element[:element] and (topmost[:d] < element[:d])
        end
        return topmost[:element]
    end
end
