$max_dim = 32

class UIElement
    attr_accessor :x, :y, :w, :h
    attr_accessor :lay_dims, :lay_pos

    def initialize(args={})
        self.x ||= 0
        self.y ||= 0
        self.w ||= 0
        self.h ||= 0
    end

    attr_writer :on_top
    def on_top?; @on_top || false; end

    # The dirty flag is used to inform the UIManager that an element's
    #  renderables should be recreated (as well as any dependent elements
    #  that were created by the looknfeel)
    attr_writer :dirty
    def dirty?; @dirty || false; end

    def update_derived_values(parent=nil)
        # Update the absolute position
        @abs_pos = parent.nil? ? [self.x, self.y] : [parent.x+self.x, parent.y+self.y]

        # Update the derived values of the dependents
        self.dependents.each do |dependent|
            dependent.update_derived_values(self)
        end

        # Update the model matrices of the renderables
        self.renderables.each do |renderable|
            renderable.set_translation_matrix(@abs_pos[0], @abs_pos[1], 0)
        end
    end

    # Dependent management
    def dependents;           @dependents ||= [];           end
    def add_dependent(dep);   self.dependents << dep;       end
    def add_dependents(deps); self.dependents.concat(deps); end
    def delete_dependents;    @dependents = [];             end

    # Renderable management
    def get_renderables(top=[])
        ret = []
        self.dependents.each do |dep|
            ret += dep.get_renderables(top)
        end
        if on_top?
            top.concat(self.renderables)
            ret
        else
            self.renderables + ret
        end
    end
    def renderables;                @renderables ||= [];            end
    def add_renderable(renderable); self.renderables << renderable; end
    def add_renderables(rends);     self.renderables.concat(rends); end
    def delete_renderables;         @renderables = [];              end

    def inspect
        super+":"+[self.x,self.y,self.w,self.h].inspect
    end
end
class UIPane < UIElement
    def update_derived_values(parent=nil)
        super(parent) unless parent.nil?
        self.children.each do |child|
            child.update_derived_values(parent)
        end
    end

    # Renderable management
    def get_renderables(top=[])
        ret = []
        self.children.each do |child|
            ret += child.get_renderables(top)
        end
        super(top) + ret
    end

    # Child management
    def children;         @children ||= [];       end
    def add_child(child); self.children << child; end
    def delete_children;  @children = [];         end
    def delete_child(elem)
        if self.children.delete(elem).nil?
            self.children.each { |child| child.delete_child(elem) if child.respond_to?(:delete_child) }
        end
    end
end

class Label < UIElement
    attr_accessor :color
    attr_writer :text
    def text; @text || ""; end
    def inspect; super + " " + self.text.inspect; end
end
class Title < Label; end

class InputField < UIElement
    attr_writer :text

    def initialize(args={}, &block)
        @on_return = block if block_given?
        super(args)
    end

    def text; @text || ""; end

    def on_click(args={})
        Event.add_listeners(self)
    end

    def input_event(event)
        return if event.type != :keyboard

        if event.key == Keyboard.KEY_RETURN
            Event.remove_listeners(self)
            @on_return.call(self.text) unless @on_return.nil?
            self.text = ""
        end

        self.text += event.ascii
        self.dirty = true
    end
end

class Button < UIElement
    attr_accessor :text
    attr_writer :on_click, :on_release

    def initialize(*args, &block)
        super(*args)
        self.on_click = block if block_given?
    end

    def on_click(args={}, &block)
        @on_click.call(args) { yield if block_given? } unless @on_click.nil?
    end

    def on_release(args={}, &block)
        @on_release.call(args) { yield if block_given? } unless @on_release.nil?
    end

    def inspect; super + " " + self.text.inspect; end
end
class InvisibleButton < Button; end
class Link < Button; end

class Slider < UIElement
    attr_accessor :cursor_pos,
                  :current_value,
                  :slider_values,
                  :continuous

    def initialize(args={}, &block)
        # Determine whether the slider moves in steps
        self.continuous = args[:continuous] || false

        # Setup the range of values the slider can take on
        args[:values] ||= [0, 1]
        self.slider_values = (args[:values].class == Array) ? args[:values] : args[:values].to_a
        self.slider_values = [self.slider_values.min, self.slider_values.max] if self.continuous

		# Set up the callback if provided
		@on_set = block if block_given?
		@on_set ||= args[:on_set]

        super(args)
    end

    def on_click(args={})
        Event.add_listeners(self)
    end

    def on_release(args={})
        Event.remove_listeners(self)
    end

    def input_event(event)
        return if event.type != :move

        self.cursor_pos = [
            event.absX - element.x,
            event.absY - element.y
        ]
        self.dirty = true
    end

	def current_value=(val)
		@on_set.call(val) if @on_set
		@current_value = val
	end
end

class Mouse < UIElement; end
