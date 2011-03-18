require 'UIElement'
require 'LookNFeel'
#require 'Reticule'

class UIManager < MHUIManager
    attr_accessor :active_element, :mouse
    def initialize(width=0, height=0)
        @active = false
        @active_element = nil

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
        Event.remove_listeners(@ap)
    end

    # Originally called on the root element, recursing down to the leaves
    def update(elapsed, element=self.root)
        # Check to see if the element is dirty, update its renderables if it is
        if element.dirty?
            @looknfeel.prepare_element(element, self)
            element.dirty = false
        end

        # Check on its children as well
        element.each_child do |child|
            update(elapsed, child)
        end
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
        when MousePressed
            @active_element = top_clickable_at(@mouse.x, @mouse.y)
            if @active_element && @active_element.respond_to?(:on_click)
                @active_element.on_click
                return :handled
            end
        when MouseReleased
            @active_element = top_clickable_at(@mouse.x, @mouse.y)
            if @active_element && @active_element.respond_to?(:on_release)
                @active_element.on_release
                return :handled
            end
        when MouseMoved
            if @cursor
                @mouse.x = [[@mouse.x + event.relX, 0].max, self.width ].min
                @mouse.y = [[@mouse.y - event.relY, 0].max, self.height].min
            end
            return :unhandled
        when KeyPressed
            if @active_element and @active_element.respond_to?(:input_event)
                status = @active_element.input_event(event)
                return status if status == :handled
            end
        end
        return :unhandled
    end

    def top_clickable_at(x, y)
        candidates = collect_clickables_at(self.root, x, y)
        sorted_candidates = candidates.compact.sort { |x,y| x <=> y }
        (sorted_candidates.last || {})[:element]
    end
    def collect_clickables_at(element, x, y, depth=0)
        clickables = []
        if element.respond_to?(:on_click) &&
            ((element.x)...(element.x+element.w)) === x &&
            ((element.y)...(element.y+element.h)) === y
            # This element is clickable and is within the appropriate range
            clickables << {
                :element=>element,
                :depth=>depth
            }
        end

        local_x = x - element.x
        local_y = y - element.y
        element.each_child do |child|
            clickables.concat(collect_clickables_at(child, local_x, local_y, depth+1))
        end

        clickables
    end

    # Element creation method
    # Creates an element of type klass, using the args hash to configure it, and possibly passing it a block
    def create(klass, args={}, material=nil, &block)
        $logger.info "Creating a #{klass.inspect}"
        object = klass.new() { |*params| block.call(*params) if block_given? }
        args.each_pair { |k,v| object.send("#{k}=", v) }

        # Since UIElements no longer create themselves, we can have the UI compute their pixel dimensions
        #  before passing them to the LookNFeel, which does the actual renderable creation
        # Compute pixel dimensions for passing to the LookNFeel
        if object.lay_dims
            object.w = object.lay_dims[0] * (self.width.to_f  / $max_dim)
            object.h = object.lay_dims[1] * (self.height.to_f / $max_dim)
        end
        if object.lay_pos
            object.x = object.lay_pos[0] * (self.width.to_f  / $max_dim)
            object.y = object.lay_pos[1] * (self.height.to_f / $max_dim)
        end

        # Flag this object as needing its renderables regenerated
        object.dirty = true

        object
    end
end
