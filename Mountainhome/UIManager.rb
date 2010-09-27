require 'UIElement'
#require 'Reticule'

class LookNFeel
    def initialize
        @materials = {
            :default   => "",
            Box        => "white",
            Button     => "transparent_grey.material",
            CheckBox   => "white",
            Clickable  => "grey",
            InfoDialog => "grey",
            InputField => "white",
            Mouse      => "cursor.material",
            Pane       => "transparent_grey.material",
        }

        @fonts = {
            :default => "",
            Title    => "big.font",
        }
    end

    def material_for(klass); @materials[klass] || @materials[:default]; end
    def font_for(klass); @fonts[klass] || @fonts[:default]; end
    def lay_divisions; 32; end
end

class UIManager < MHUIManager
    attr_accessor :active_element, :focus_override, :looknfeel
    def initialize(looknfeel, core)
		super(looknfeel, core)

        # TODO - actually setup the looknfeel
        @looknfeel = LookNFeel.new

        @active = false
        @active_element = nil
        @focus_override = nil

        max_dim = @looknfeel.lay_divisions
        self.root = create(UIElement)
        @mouse    = create(Mouse, {:parent => self.root})

        @persistent_elems = []
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
        self.root.compute_dimensions
    end

    def update(elapsed)
        # Update elements
        self.root.update(elapsed)
    end

    def input_event(event)
        case event
        when MousePressed
            hit = element_at(@mouse.x, @mouse.y)
            if hit
                $logger.info "User clicked on UIElement #{hit.inspect}"
                @active_element = hit
                @active_element.on_click { @mouse.x }
            else
                @selection = create(Pane, {:anchor => [@mouse.x, @mouse.y], :parent => self.root})
            end
            return :handled
        when MouseReleased
            if @selection
                kill_element(@selection)
                @selection = nil
            end
            if @active_element and @active_element.respond_to? "on_release"
                @active_element.on_release
            end
            return :handled
        when MouseMoved
            @mouse.x = [[@mouse.x + event[:relX], 0].max, self.width ].min
            @mouse.y = [[@mouse.y - event[:relY], 0].max, self.height].min
            (@selection.resize(@mouse.x-@selection.x, @mouse.y-@selection.y)) if @selection
            return :handled
        when KeyPressed
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

    # Element creation method
    # Creates an element of type klass, using the args hash to configure it, and possibly passing it a block
    def create(klass, args={}, material=nil, &block)
        name     = "#{klass}#{args.hash}"
        material = material || @looknfeel.material_for(klass)
        font     = @looknfeel.font_for(klass)

        object = klass.new(name, self, material, font) { |*params| block.call(*params) if block_given? }

        args.each_pair { |k,v| object.send("#{k}=", v) }
        object.manager = self
        object.compute_dimensions

        return object
    end
end
