class Event < Hash
    def self.hash_attr(*names)
        names.each do |name|
            class_eval "def #{name}()     self[:#{name}]       end"
            class_eval "def #{name}=(val) self[:#{name}] = val end"
        end
    end

    @@uppercase_punc = {
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
    
    @@printable = [Keyboard.KEY_BACKSPACE, Keyboard.KEY_TAB, Keyboard.KEY_CLEAR, Keyboard.KEY_RETURN,
        (Keyboard.KEY_SPACE..Keyboard.KEY_AT).to_a,
        (Keyboard.KEY_LEFTBRACKET..Keyboard.KEY_RCURLY).to_a].flatten

    ############################
    # Instance Stuff           #
    ############################

    hash_attr :type, :state

    def initialize(hash)
        hash.each_pair {|key, value| self[key] = value }
        flatten!
    end

    # Generally, if either l or r mod keys are pressed we want to use the canonical modifier
    # If there are any special cases this field can be changed after Event creation.
    def flatten
        event = self.dup
        case event[:modifier]
        when Keyboard.MOD_LCTRL, Keyboard.MOD_RCTRL
            event[:modifier] = Keyboard.MOD_CTRL
        when Keyboard.MOD_LSHIFT, Keyboard.MOD_RSHIFT
            event[:modifier] = Keyboard.MOD_SHIFT
        when Keyboard.MOD_LALT, Keyboard.MOD_RALT
            event[:modifier] = Keyboard.MOD_ALT
        when Keyboard.MOD_LMETA, Keyboard.MOD_RMETA
            event[:modifier] = Keyboard.MOD_META
        end
        event
    end

    def flatten!
        case self[:modifier]
        when Keyboard.MOD_LCTRL, Keyboard.MOD_RCTRL
            self[:modifier] = Keyboard.MOD_CTRL
        when Keyboard.MOD_LSHIFT, Keyboard.MOD_RSHIFT
            self[:modifier] = Keyboard.MOD_SHIFT
        when Keyboard.MOD_LALT, Keyboard.MOD_RALT
            self[:modifier] = Keyboard.MOD_ALT
        when Keyboard.MOD_LMETA, Keyboard.MOD_RMETA
            self[:modifier] = Keyboard.MOD_META
        end
    end

    def printable?
        @@printable.include?(self.key)
    end
    
    # side effect is that LHS gets flattened.
    def shifted?
        case self[:modifier]
        when Keyboard.MOD_LSHIFT, Keyboard.MOD_RSHIFT, Keyboard.MOD_SHIFT
            return true
        end
        return false
    end

    # Change the key to the correct shifted character
    def convert_shift
        return self unless shifted?
        # revise uppercase punctuation
        self.key = @@uppercase_punc[self.key] if @@uppercase_punc[self.key]

        # revise uppercase letters
        self.key -= 32 if Keyboard.KEY_a <= self.key and self.key <= Keyboard.KEY_z
        self
    end
end

class KeyPressed < Event
    hash_attr :key, :modifier
    def initialize(key, modifier)
        self.type = :keyboard; self.state = :pressed
        self.key = key; self.modifier = modifier
        flatten!
        $logger.info("key_pressed #{self.inspect}")
    end
end

class KeyReleased < Event
    hash_attr :key, :modifier
    def initialize(key, modifier)
        self.type = :keyboard; self.state = :released
        self.key = key; self.modifier = modifier
        flatten!
        $logger.info("key_released #{self.inspect}")
    end
end

class MousePressed < Event
    hash_attr :button, :x, :y
    def initialize(button, x, y)
        self.type = :mouse; self.state = :pressed
        self.button = button; self.x = x; self.y = y
    end
end

class MouseReleased < Event
    hash_attr :button, :x, :y
    def initialize(button, x, y)
        self.type = :mouse; self.state = :released
        self.button = button; self.x = x; self.y = y
    end
end

class MouseMoved < Event
    hash_attr :absX, :absY, :relX, :relY
    def initialize(absX, absY, relX, relY)
        self.type = :move
        self.absX = absX; self.absY = absY; self.relX = relX; self.relY = relY
    end
end

class Event < Hash
    def self.key_pressed(key, modifier = 0)
        KeyPressed.new(key, modifier)
    end

    def self.key_released(key, modifier = 0)
        KeyReleased.new(key, modifier)
    end

    def self.mouse_pressed(button, x, y)
        MousePressed.new(button, x, y)
    end

    def self.mouse_released(button, x, y)
        MouseReleased.new(button, x, y)
    end

    def self.mouse_moved(absX, absY, relX, relY)
        MouseMoved.new(absX, absY, relX, relY)
    end
end