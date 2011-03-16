$max_dim = 32

class UIElement < MHUIElement
    attr_accessor :lay_dims, :lay_pos
    def inspect
        super + " : " + [self.x,self.y,self.w,self.h].inspect
    end
end

class Label < UIElement
    attr_accessor :color
    attr_writer :text
    def text; @text || ""; end
end
class Title < Label; end

class InputField < UIElement
    def input_event(event)
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
        self.on_click.call(args) { yield if block_given? } unless self.on_click.nil?
    end

    def on_release(args={}, &block)
        self.on_release.call(args) { yield if block_given? } unless self.on_release.nil?
    end
end
class InvisibleButton < Button; end
class Link < Button; end

class Slider < UIElement
    attr_writer :values, :current_value, :on_set

    def initialize(*args, &block)
        super(*args)
        self.on_set = block if block_given?
    end

    def values; @values || []; end

    def current_value; @current_value || self.values.first; end

    def on_set(value)
        self.current_value = value
        self.on_set.call(value) unless self.on_set.nil?
    end
end

class Grouping < UIElement
    attr_accessor :type, :sub_elements, :sub_element_class, :shared_attributes
end

class Mouse < UIElement; end
