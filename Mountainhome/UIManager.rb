require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
		super(args[:looknfeel])

        # FIXME - actually setup the looknfeel
        @default_material = "white"

        # Set up the root element
        @root = add_element("root", 0, 0, self.width, self.height, {:mat => ""})

        # Set up the mouse pointer
        @mouse = add_element("mouse", 0, 0, 14, 21, {:mat => "cursor"})
        @mouse.set_offset(0, -21)
        @mouse.always_on_top

        # Set up a console
        eval_proc = args[:eval_proc] || Proc.new { |cmd| $logger.info "No evaluator specified, ignoring input #{cmd}" }
        @console = Console.new(self, eval_proc)

        # Manager state
        @ticks = Array.new(50, 0)

        @active = true
        @active_element = nil
    end

    # This call is for menu builders, and is used to clear everything except the root and mouse elements
    def clear_elements
        @root.cull_children([@mouse] + @console.elements)
    end
    
    def teardown
        $logger.info "Tearing down UIManager"
    end

    def update(elapsed)
        # Keep track of the updates/second
        @ticks = @ticks[1..-1] + [elapsed]
        @avg_tick = @ticks.inject { |sum, t| sum ? sum+t : t } / @ticks.size

        # Update elements
        @root.update(elapsed)
    end

    def input_event(args={})
        case args[:type]
        when :mouse
			case args[:state]
			when :pressed
                hit = element_at(@mouse.x, @mouse.y)
                if hit
                    $logger.info "User clicked on UIElement #{hit.inspect}"
                    hit.on_click
                else
                    @selection = add_element("select", @mouse.x, @mouse.y, 0, 0,
                                             {:mat => "t_grey"})
                end
			when :released
                if @selection
                    kill_element(@selection)
                    @selection = nil
                end
			end
			return :handled
		when :move
            return :unhandled if (not @active)

			@mouse.x = [[@mouse.x + args[:x], 0].max, self.width ].min
			@mouse.y = [[@mouse.y - args[:y], 0].max, self.height].min
            
            (@selection.resize(@mouse.x-@selection.x, @mouse.y-@selection.y)) if @selection
			return :handled
        when :keyboard
            status = :unhandled

            if @active_element and args[:state] == :pressed
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

    def add_element(name, x, y, w, h, opt={})
        # Check for optional arguments and do appropriate setup
        element_type = opt[:element_type] || UIElement
        mat          = opt[:mat]          || @default_material
        text         = opt[:text]         || "" 
        parent       = opt[:parent]       || @root
        # Clear out optional args that have been handled
        [:parent, :element_type, :mat, :text].each { |h| opt.delete(h) }

        # Call constructor and setup basic properties
        n_elem = element_type.new(name, self, mat, text, opt)
        n_elem.set_dimensions(x,y,w,h)

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
