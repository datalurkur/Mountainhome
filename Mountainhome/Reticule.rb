class Reticule < UIElement
    def initialize(name, manager, x, y, w, h, args={})
        super(name, manager, "blue", "", "", args)
        set_dimensions(x, y, w, h)
    end

    def push_char(event)
        self.text = (self.text + [event.convert_shift.key].pack("C"))
        # Seems to follow a printf format, where "%%" resolves to '%'
        if event.key == Keyboard.KEY_PERCENT
            self.text = (self.text + [event.convert_shift.key].pack("C"))
        end
    end

    def pop_char
        self.text = self.text[0..-2]
    end
end

