=begin
NOTES
    -Thinking the element alignment is going to go away, in favor of letting the looknfeel care about things like that
        Not sure how this is going to manifest yet, since I can think of certain cases where it might be useful to specify alignment
        It might be easiest to just pick one convention and stick with it (example: the element position always indicates its lower-left corner)
=end

$max_dim = 32

class UIElement < MHUIElement
    attr_writer :ldims
    def ldims; @ldims ||=[0,0,0,0]; end

    def inspect
        super + " : " + [self.x,self.y,self.w,self.h].inspect
    end
end

class Label < UIElement
    attr_accessor :text
end

class InputField < UIElement
end

class Button < UIElement
    attr_accessor :text
end

class Slider < UIElement
end

class Grouping < UIElement
end

class Mouse < UIElement; end
