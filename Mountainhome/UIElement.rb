=begin
NOTES
    -Thinking the element alignment is going to go away, in favor of letting the looknfeel care about things like that
        Not sure how this is going to manifest yet, since I can think of certain cases where it might be useful to specify alignment
        It might be easiest to just pick one convention and stick with it (example: the element position always indicates its lower-left corner)
=end

$max_dim = 32

class UIElement < MHUIElement
    attr_accessor :lay_dims, :lay_pos
    def inspect
        super + " : " + [self.x,self.y,self.w,self.h].inspect
    end
end

class Label < UIElement
    attr_accessor :text, :color
end
class Title < Label; end

class InputField < UIElement
end

class Button < UIElement
    attr_accessor :text
    attr_writer   :on_click

    def initialize(*args, &block)
        super(*args)
        self.on_click=block if block_given?
    end

    def on_click(args={}, &block)
        unless @on_click.nil?
            @on_click.call(args) { yield if block_given? }
        end
    end
end
class Link < Button; end

class Slider < UIElement
end

class Grouping < UIElement
    attr_accessor :type, :sub_elements, :sub_element_class, :shared_attributes
end

class Mouse < UIElement; end
