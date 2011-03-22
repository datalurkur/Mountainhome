$max_dim = 32

class UIElement < MHUIElement
    attr_accessor :lay_dims, :lay_pos

    # The dirty flag is used to inform the UIManager that an element's
    #  renderables should be recreated (as well as any dependent elements
    #  that were created by the looknfeel)
    attr_writer :dirty
    def dirty?; @dirty || false; end

    # Dependents are sub-elements created by the looknfeel that
    #  should be recreated when an element is marked dirty
    attr_writer :dependent
    def dependent?; @dependent || false; end
    def delete_dependents
        deps = []
        self.each_child do |child|
            deps << child if child.dependent?
        end
        deps.each do |dep|
            self.delete_child(dep)
        end
    end

    def inspect
        super + " : " + [self.x,self.y,self.w,self.h].inspect
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
    def text; @text || ""; end

    def input_event(event)
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
    attr_writer :values, :current_value, :set

    def initialize(*args, &block)
        super(*args)
        self.set = block if block_given?
    end

    def values; @values || []; end

    def current_value; @current_value || self.values.first; end

    def set(value)
        self.current_value = value
        self.dirty = true
        @set.call(value) unless @set.nil?
    end
end

class Grouping < UIElement
    attr_accessor :type, :sub_elements, :sub_element_class, :shared_attributes
end

class Mouse < UIElement; end
