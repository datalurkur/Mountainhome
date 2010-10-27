UPPERCASE_PUNCTUATION = {
    Keyboard.KEY_1 => Keyboard.KEY_EXCLAIM,
    Keyboard.KEY_2 => Keyboard.KEY_AT,
    Keyboard.KEY_3 => Keyboard.KEY_HASH,
    Keyboard.KEY_4 => Keyboard.KEY_DOLLAR,
    Keyboard.KEY_5 => Keyboard.KEY_PERCENT,
    Keyboard.KEY_6 => Keyboard.KEY_CARET,
    Keyboard.KEY_7 => Keyboard.KEY_AMPERSAND,
    Keyboard.KEY_8 => Keyboard.KEY_ASTERISK,
    Keyboard.KEY_9 => Keyboard.KEY_LEFTPAREN,
    Keyboard.KEY_0 => Keyboard.KEY_RIGHTPAREN,

    Keyboard.KEY_MINUS => Keyboard.KEY_UNDERSCORE,
    Keyboard.KEY_EQUALS => Keyboard.KEY_PLUS,

    Keyboard.KEY_LEFTBRACKET => Keyboard.KEY_LCURLY,
    Keyboard.KEY_RIGHTBRACKET => Keyboard.KEY_RCURLY,
    Keyboard.KEY_BACKSLASH => Keyboard.KEY_PIPE,

    Keyboard.KEY_SEMICOLON => Keyboard.KEY_COLON,
    Keyboard.KEY_QUOTE => Keyboard.KEY_QUOTEDBL,

    Keyboard.KEY_COMMA => Keyboard.KEY_LESS,
    Keyboard.KEY_PERIOD => Keyboard.KEY_GREATER,
    Keyboard.KEY_SLASH => Keyboard.KEY_QUESTION,
}

PRINTABLE = [Keyboard.KEY_BACKSPACE, Keyboard.KEY_TAB, Keyboard.KEY_CLEAR, Keyboard.KEY_RETURN,
            (Keyboard.KEY_SPACE..Keyboard.KEY_AT).to_a,
            (Keyboard.KEY_LEFTBRACKET..Keyboard.KEY_RCURLY).to_a].flatten

class Event < Hash
    # Set up getters for hash values.
    def self.hash_attr_reader(*names)
        names.each do |name|
            class_eval "def #{name}()     self[:#{name}]       end"
        end
    end

    # These values exist for every Event.
    hash_attr_reader :type, :state
end

class KeyboardEvent < Event
    hash_attr_reader :key, :modifier

    def initialize(key = 0, modifier = 0)
        self[:type] = :keyboard
        self[:key] = key; self[:modifier] = modifier
        flatten!
        # Error out if we try to create an instance of this class rather than a subclass.
        throw NotImplementedError if self.state.nil?
    end

    # If either left or right modifier keys are pressed we want to use the canonical modifier
    # If there are any special cases this field can be changed after Event creation.
    def flatten!
        [Keyboard.MOD_SHIFT, Keyboard.MOD_CTRL,
         Keyboard.MOD_ALT, Keyboard.MOD_META].each do |mod|
            self[:modifier] = mod if modifier & mod != 0
        end
        self
    end

    def flatten
        self.dup.flatten!
    end

    # Change the key to the correct shifted character
    def upper!
        return self unless shift_held?
        # revise uppercase punctuation
        self[:key] = UPPERCASE_PUNCTUATION[self.key] if UPPERCASE_PUNCTUATION[self.key]

        # revise uppercase letters
        self[:key] -= 32 if (Keyboard.KEY_a..Keyboard.KEY_z).to_a.include?(self.key)
        self
    end

    def upper
        self.dup.upper!
    end

    #############################################
    # Boolean event checkers for KeyboardEvent. #
    #############################################
    def printable?
        PRINTABLE.include?(self.key)
    end
    
    def shift_held?
        return self.modifier & Keyboard.MOD_SHIFT != 0
    end

    def control_held?
        return self.modifier & Keyboard.MOD_CTRL != 0
    end

    def alt_held?
        return self.modifier & Keyboard.MOD_ALT != 0
    end

    def meta_held?
        return self.modifier & Keyboard.MOD_META != 0
    end
end

class KeyPressed < KeyboardEvent
    def initialize(key = 0, modifier = 0)
        self[:state] = :pressed
        super
    end
end

class KeyReleased < KeyboardEvent
    def initialize(key = 0, modifier = 0)
        self[:state] = :released
        super
    end
end

class MouseButtonEvent < Event
    hash_attr_reader :button, :x, :y
    def initialize(button = 0, x = 0, y = 0)
        self[:type] = :mouse
        self[:button] = button; self[:x] = x; self[:y] = y
        # Error out if we try to create an instance of this class rather than a subclass.
        throw NotImplementedError if self.state.nil?
    end
end

class MousePressed < MouseButtonEvent
    def initialize(button = 0, x = 0, y = 0)
        self[:state] = :pressed
        super
    end
end

class MouseReleased < MouseButtonEvent
    def initialize(button = 0, x = 0, y = 0)
        self[:state] = :released
        super
    end
end

class MouseMoved < Event
    hash_attr_reader :absX, :absY, :relX, :relY
    def initialize(absX = 0, absY = 0, relX = 0, relY = 0)
        self[:type] = self[:state] = :move
        self[:absX] = absX; self[:absY] = absY; self[:relX] = relX; self[:relY] = relY
    end
end

class Event < Hash
    def self.key_pressed(key = 0, modifier = 0)
        KeyPressed.new(key, modifier)
    end

    def self.key_released(key = 0, modifier = 0)
        KeyReleased.new(key, modifier)
    end

    def self.mouse_pressed(button = 0, x = 0, y = 0)
        MousePressed.new(button, x, y)
    end

    def self.mouse_released(button = 0, x = 0, y = 0)
        MouseReleased.new(button, x, y)
    end

    def self.mouse_moved(absX = 0, absY = 0, relX = 0, relY = 0)
        MouseMoved.new(absX, absY, relX, relY)
    end
end