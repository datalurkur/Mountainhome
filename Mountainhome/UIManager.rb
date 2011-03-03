require 'UIElement'
require 'LookNFeel'
#require 'Reticule'

class UIManager < MHUIManager
    attr_accessor :active_element, :focus_override, :mouse
    def initialize(width=0, height=0)
        @active = false
        @active_element = nil
        @focus_override = nil

        self.width  = width
        self.height = height

        # For now, create a default LookNFeel
        @looknfeel = LookNFeel.new

        self.root = create(UIElement)
        @mouse    = create(Mouse, {:parent => self.root})

        @cursor = true

        @persistent_elems = [@mouse]
    end

    # This call is for menu builders, and is used to clear everything except the root and mouse elements
    def clear_elements(clear_all = false)
        children_to_clear = []
        self.root.each_child do |child|
            unless @persistent_elems.include?(child) && !clear_all
                children_to_clear << child
            end
        end

        children_to_clear.each do |child|
            self.root.delete_child(child)
        end
    end
    
    def teardown
        $logger.info "Tearing down UIManager"
    end

    def update(elapsed)
        # Update elements
        # FIXME - Elements currently have no reason to update, add this back in when/if they do
        #self.root.update(elapsed)
    end

    def toggle_cursor
        @cursor = !@cursor
        if @cursor
            @mouse = create(Mouse, {:parent => self.root})
            @persistent_elems << @mouse
        else
            @persistent_elems.delete(@mouse)
            self.root.delete_child(@mouse)
            @mouse = nil
        end
    end

    def input_event(event)
        case event
=begin
        when MousePressed
            # Set the active element to the highest depth UI element, or nil.
            @active_element = element_at(@mouse.x, @mouse.y)
            if @active_element
                $logger.info "User clicked on UIElement #{@active_element.inspect}"
                # Sliders need to know the x value of the mouse until the mouse button is released.
                return @active_element.on_click { @mouse.x } if @active_element.respond_to?(:on_click)
            else
                kill_element(@selection) unless @selection.nil?
                @selection = create(Pane, {:anchor => [@mouse.x, @mouse.y], :parent => self.root})
            end
        when MouseReleased
            if @selection
                kill_element(@selection) unless @selection.nil?
                @selection = nil
            end
            if @active_element and @active_element.respond_to?(:on_release)
                @active_element.on_release
            end
=end
        when MouseMoved
            if @cursor
                @mouse.x = [[@mouse.x + event.relX, 0].max, self.width ].min
                @mouse.y = [[@mouse.y - event.relY, 0].max, self.height].min
            end
            #@selection.resize(@mouse.x-@selection.x, @mouse.y-@selection.y) unless @selection.nil?

            # Moving the mouse pointer shouldn't kill any other effects of mouse movement.
            return :unhandled
=begin
        when KeyPressed
            if @active_element and @active_element.respond_to?(:input_event)
                return @active_element.input_event(event)
            end
=end
        end
        return :unhandled
    end

=begin
    def kill_element(elem)
        $logger.info "Culling #{elem}"
        self.root.delete_child(elem)
    end
=end

    # Find the topmost menu element at [x,y]
=begin
    def element_at(x, y)
        elems = self.root.elements_at(x,y,0)
        topmost = {:element => nil, :d => -1}
        elems.each do |element|
            topmost = element if element[:element] and (topmost[:d] < element[:d])
        end
        return topmost[:element] if @focus_override.nil? or @focus_override.include?(topmost[:element])
    end
=end

    # Element creation method
    # Creates an element of type klass, using the args hash to configure it, and possibly passing it a block
    def create(klass, args={}, material=nil, &block)
        # Create the UIElement
        object = klass.new() { |*params| block.call(*params) if block_given? }
        args.each_pair { |k,v| object.send("#{k}=", v) }

        # AJEAN - Since UIElements no longer create themselves, we can have the UI compute their pixel dimensions
        #  before passing them to the LookNFeel, which does the actual renderable creation
        # Compute pixel dimensions for passing to the LookNFeel
        dims = []
        object.ldims.each_with_index do |dim, index|
            dims << if index.even?
                (dim * (self.width.to_f  / $max_dim))
            else
                (dim * (self.height.to_f / $max_dim))
            end
        end

        # Call C object bindings
        # If ldims aren't specified, leave the position alone
        object.x = dims[0] if dims[0]
        object.y = dims[1] if dims[1]
        # Only required for Ruby
        object.w = dims[2] if dims[2]
        object.h = dims[3] if dims[3]

        # Call on the looknfeel
        @looknfeel.prepare_element(object, self)

        object
    end
end
