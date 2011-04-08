require 'UIElement'
require 'LookNFeel'
#require 'Reticule'

class UIManager < UIPane
    attr_accessor :mouse

    def initialize(width=0, height=0)
        @active = false

        self.w = width
        self.h = height

        # For now, create a default LookNFeel
        @looknfeel = LookNFeel.new(self)

        @persistent_elems = Array.new

        self.cursor_enabled = true

        super()
    end

    # This call is for menu builders, and is used to clear everything except the root and mouse elements
    def clear_elements(clear_all = false)
        children_to_clear = []
        self.children.each do |child|
            unless @persistent_elems.include?(child) && !clear_all
                children_to_clear << child
            end
        end

        children_to_clear.each do |child|
            self.delete_child(child)
        end
    end
    
    # Originally called on the root element, recursing down to the leaves
    def update(elapsed, element=self)
        # Check to see if the element is dirty, update its renderables if it is
        if element.dirty?
            @looknfeel.prepare_element(element)
            element.dirty = false
        end

        # Check on its dependents
        element.dependents.each do |dependent|
            update(elapsed, dependent)
        end

        # Check on its children as well
        if element.respond_to?(:children)
            element.children.each do |child|
                update(elapsed, child)
            end
        end
    end

    def render(context)
        self.update_derived_values

        top_renderables = []
        total_renderables = self.get_renderables(top_renderables)

        context.render_2d(self.w, self.h, total_renderables + top_renderables)
    end

    def cursor_enabled=(value)
        if @cursor != value
            @cursor = value
            if @cursor
                @mouse = create(Mouse)
                @persistent_elems << @mouse
            else
                @persistent_elems.delete(@mouse)
                self.delete_child(@mouse)
                @mouse = nil
            end
        end
    end

    def toggle_cursor
        self.cursor_enabled = !@cursor
    end

    def input_event(event)
        case event
        when MousePressed
            if @cursor
                clicked_element = top_clickable_at(@mouse.x, @mouse.y)
                if clicked_element && clicked_element.respond_to?(:on_click)
                    clicked_element.on_click
                    return :handled
                end
            end
        when MouseReleased
            if @cursor
                clicked_element = top_clickable_at(@mouse.x, @mouse.y)
                if clicked_element && clicked_element.respond_to?(:on_release)
                    clicked_element.on_release
                    return :handled
                end
            end
        when MouseMoved
            if @cursor
                @mouse.x = [[@mouse.x + event.relX, 0].max, self.w].min
                @mouse.y = [[@mouse.y - event.relY, 0].max, self.h].min
            end
        end
        return :unhandled
    end

    def top_clickable_at(x, y)
        candidates = collect_clickables_at(self, x, y)
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

        if element.respond_to?(:children)
            local_x = x - element.x
            local_y = y - element.y
            element.children.each do |child|
                clickables.concat(collect_clickables_at(child, local_x, local_y, depth+1))
            end
        end

        clickables
    end

    # Element creation method
    # Creates an element of type klass, using the args hash to configure it, and possibly passing it a block
    def create(klass, args={}, parent=nil, &block)
        #$logger.info "Creating #{klass} with args #{args.inspect}"
        object = klass.new() { |*params| block.call(*params) if block_given? }

        # Setup the parent (whether this be a dependent or a child)
        if args.delete(:dependent)
            dependee = parent || self
            dependee.add_dependent(object)
        else
            parent ||= self
            parent.add_child(object) if parent.respond_to?(:add_child)
        end

        # Send the args to the newly created object
        args.each_pair { |k,v| object.send("#{k}=", v) }

        # Since UIElements no longer create themselves, we can have the UI compute their pixel dimensions
        #  before passing them to the LookNFeel, which does the actual renderable creation
        # Compute pixel dimensions for passing to the LookNFeel
        if object.lay_dims
            object.w = object.lay_dims[0] * (self.w.to_f  / $max_dim)
            object.h = object.lay_dims[1] * (self.h.to_f / $max_dim)
        end
        if object.lay_pos
            object.x = object.lay_pos[0] * (self.w.to_f  / $max_dim)
            object.y = object.lay_pos[1] * (self.h.to_f / $max_dim)
        end

        # Flag this object as needing its renderables regenerated
        object.dirty = true

        object
    end
end
