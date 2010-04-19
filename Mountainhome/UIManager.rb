require 'UIElement'

class UIManager < MHUIManager
    attr_accessor :active_element
    def initialize(looknfeel, core)
		super(looknfeel, core)

        # FIXME - actually setup the looknfeel
        @default_material = "white"

        @active = false
        @active_element = nil

        self.root = Invisible.new("root", self)
        @mouse = Mouse.new(self)
        self.root.add_child(@mouse)
    end

    # This call is for menu builders, and is used to clear everything except the root and mouse elements
    def clear_elements(clear_all = false)
        self.root.cull_children if self.root
    end
    
    def teardown
        $logger.info "Tearing down UIManager"
    end

    def resize(width, height)
        self.root.set_dimensions(0, 0, width, height)
    end

    def update(elapsed)
        # Update elements
        self.root.update(elapsed)
    end

    def input_event(args={})
        case args[:type]
        when :mouse
			case args[:state]
			when :pressed
                hit = element_at(@mouse.x, @mouse.y)
                if hit
                    $logger.info "User clicked on UIElement #{hit.inspect}"
                    @active_element = hit
                    @active_element.on_click(:click) { @mouse.x }
                else
                    @selection = Pane.new("selection", self, @mouse.x, @mouse.y, 0, 0, {:parent => self.root})
                end
			when :released
                if @selection
                    kill_element(@selection)
                    @selection = nil
                end
                if @active_element
                    @active_element.on_click(:release)
                    @active_element = nil
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

    def kill_element(elem)
        $logger.info "Culling #{elem}"
        self.root.cull_child(elem)
    end

    # Find the topmost menu element at [x,y]
    def element_at(x, y)
        elems = self.root.elements_at(x,y,0)
        topmost = {:element => nil, :d => -1}
        elems.each do |element|
            topmost = element if element[:element] and (topmost[:d] < element[:d])
        end
        return topmost[:element]
    end
end
