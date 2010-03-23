require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
		super(args[:looknfeel])

        # FIXME - actually setup the looknfeel
        @default_material = "white"

        # FIXME - need to get actual dimensions here
        @width = 800
        @height = 600

        # Set up the root element
        @root = add_element("root", 0, 0, @width, @height, {:mat => ""})

        # Set up the mouse pointer
        @mouse = add_element("mouse", 0, 0, 14, 21, {:mat => "cursor"})
        @mouse.set_offset(0, -21)
        @mouse.always_on_top

        # Set up a console
        @console_text = ""
        @console = Console.new(self)

        @active = false
        @active_element = nil
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
                #    kill_element(hit)
                else
                #    name = "element"
                #    elem = new_element(:name => name, :mat => "white", :text => name,
                #                       :x => @mouse.x, :y => @mouse.y,
                #                       :w => 100, :h => 20, :clickable => true)
                #    @root.add_child(elem)
                end
			when :released
			end
			return :handled
		when :move
            return :unhandled if (not @active)

			@mouse.x = [[@mouse.x + args[:x], 0].max, @width].min
			@mouse.y = [[@mouse.y - args[:y], 0].max, @height].min
			return :handled
        when :keyboard
            status = :unhandled

            if @active_element and args[:state] == :pressed
                $logger.info "Active element absorbs input"
                status = @active_element.input_event(args)
            end

            if status == :unhandled
                case args[:key]
                when Keyboard.KEY_BACKQUOTE
                    if args[:state] == :pressed
                        @active_element = (@active_element ? nil : @console)
                        @console.toggle
                    end
                    return :handled
                when Keyboard.KEY_TAB
                    if args[:state] == :pressed
                        @active = (not @active)
                        $logger.info "Setting UIManager activity to #{@active}"
                    end
                    return :handled
                when Keyboard.KEY_UP, Keyboard.KEY_DOWN, Keyboard.KEY_LEFT, Keyboard.KEY_RIGHT
                    return :unhandled if (not @active)
                    
                    $logger.info "UIManager receives keypad input #{args[:key]}"
                    return :handled 
                else
                    $logger.info "UIManager deferred handling of #{args[:key]} to GameState"
                end
            end
            return :handled
        end
        return :unhandled
    end

    def new_element(name, x, y, w, h, opt={})
        # Set up the type of element to be created
        element_type = opt[:element_type] || UIElement
        # Check for optional arguments and do appropriate setup
        mat          = opt[:mat]          || @default_material
        text         = opt[:text]         || "" 
        # Clear out optional args that have been handled
        [:element_type, :mat, :text].each { |h| opt.delete(h) }

        # Call constructor and setup basic properties
        uie = element_type.new(name, self, mat, text, opt)
        uie.set_dimensions(x,y,w,h)

        return uie
    end

    def add_element(name, x, y, w, h, opt={})
        # Create the new element
        n_elem = new_element(name, x, y, w, h, opt)

        # Check for optional parent arg
        parent = opt[:parent] || @root
        opt.delete(:parent)

        # Attach the new element to its parent, setting up the root element if we haven't already
        if parent == @root
            @root ? @root.add_child(n_elem) : set_root(@root = n_elem)
        else
            parent.add_child(n_elem)
        end

        return n_elem
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
