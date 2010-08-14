require 'UIElement'
#require 'Reticule'

class UIManager < MHUIManager
    attr_accessor :active_element, :focus_override
    def initialize(looknfeel, core)
		super(looknfeel, core)

        # FIXME - actually setup the looknfeel
        @default_material = "white"

        @active = false
        @active_element = nil
        @focus_override = nil

        @persistent_elems = []

        self.root = Invisible.new("toplevel_root", self)

        @mouse = Mouse.new(self)
        self.root.add_child(@mouse)
        @persistent_elems << @mouse
    end

    # This call is for menu builders, and is used to clear everything except the root and mouse elements
    def clear_elements(clear_all = false)
        self.root.cull_children if self.root
        @persistent_elems.each { |elem| self.root.add_child(elem) }
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

    def input_event(event)
        case event[:type]
        when :mouse
            case event[:state]
            when :pressed
                hit = element_at(@mouse.x, @mouse.y)
                if hit
                    $logger.info "User clicked on UIElement #{hit.inspect}"
                    @active_element = hit
                    @active_element.on_click { @mouse.x }
                else
                    @selection = Pane.new("selection", self, @mouse.x, @mouse.y, 0, 0, {:parent => self.root})
                end
            when :released
                if @selection
                    kill_element(@selection)
                    @selection = nil
                end
                if @active_element and @active_element.respond_to? "on_release"
                    @active_element.on_release
                end
            end
            return :handled
        when :move
            @mouse.x = [[@mouse.x + event[:x], 0].max, self.width ].min
            @mouse.y = [[@mouse.y - event[:y], 0].max, self.height].min
            (@selection.resize(@mouse.x-@selection.x, @mouse.y-@selection.y)) if @selection
            return :handled
        when :keyboard
            if @active_element && event[:state] == :pressed
                @active_element.input_event(event)
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
        return topmost[:element] if @focus_override.nil? or @focus_override.include?(topmost[:element])
    end
end
