require 'UIElement'

class UIManager < MHUIManager
    attr_accessor :active_element
    def initialize(looknfeel, core)
		super(looknfeel, core)

        # FIXME - actually setup the looknfeel
        @default_material = "white"

        @active = false
        @active_element = nil

        @root = add_element("root", 0, 0, self.width, self.height, {:mat => ""})
        @mouse = add_element("mouse", 0, 0, 14, 21, {:mat => "cursor"})
        @mouse.set_offset(0, -21)
        @mouse.always_on_top
    end

    # This call is for menu builders, and is used to clear everything except the root and mouse elements
    def clear_elements(clear_all = false)
        #@root.cull_child(clear_all ? [] : [@mouse]) if @root
        @root.cull_children if @root
    end
    
    def teardown
        $logger.info "Tearing down UIManager"
    end

    def resize(width, height)
        @root.set_dimensions(0, 0, width, height)
    end

    def update(elapsed)
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
			@mouse.x = [[@mouse.x + args[:x], 0].max, self.width ].min
			@mouse.y = [[@mouse.y - args[:y], 0].max, self.height].min
            (@selection.resize(@mouse.x-@selection.x, @mouse.y-@selection.y)) if @selection
			return :handled
        when :keyboard
            if @active_element && args[:state] == :pressed
                @active_element.input_event(args)
                return :handled
            end
        end
        return :unhandled
    end

    def add_element(name, x, y, w, h, opt={})
        # Check for optional arguments and do appropriate setup
        element_type = opt[:element_type] || UIElement
        mat          = opt[:mat]          || @default_material
        text         = opt[:text]         || "" 
        font         = opt[:font]         || "" 
        parent       = opt[:parent]       || @root
        # Clear out optional args that have been handled
        [:parent, :element_type, :mat, :text].each { |h| opt.delete(h) }

        # Call constructor and setup basic properties
        n_elem = element_type.new(name, self, mat, font, text, opt)
        n_elem.set_dimensions(x,y,w,h)

        # Attach the new element to its parent, setting up the root element if we haven't already
        if parent == @root
            @root ? @root.add_child(n_elem) : set_root(@root = n_elem)
        else
            parent.add_child(n_elem)
        end

        n_elem
    end

    def kill_element(elem)
        $logger.info "Culling #{elem}"
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
