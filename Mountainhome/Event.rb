# Event inherits from Hash, which brings in free object comparison based on hash entries which
# are the various getters/setters.

# Event also contains functionality for accepting events passed to it from C
# and passing them to registered listeners in order.

# Some useful keyboard constants.
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
    # Failure to remove listeners sometime after they've been added could lead
    # to bizarre situations like multiple UIManagers taking events. Don't do it!
    def self.add_listeners(*listeners)
        @@listeners ||= []
        @@listeners += listeners.select {|et| et.respond_to?(:input_event) }
        $logger.info("Events now passing to #{@@listeners.inspect}")
    end

    # Return nil if any listeners weren't in the list.
    def self.remove_listeners(*listeners)
        return nil if @@listeners.nil?
        listeners.each do |listener|
            return nil if @@listeners.delete(listener).nil?
        end
        listeners
    end

    # Takes event passed from C.
    # In order, passes the event until respond_to? and returned :handled from:
    # 1) self.input_event method
    # 2) @listeners, set up via send_events_to
    def self.receive_event(event)
        $logger.info("received event #{event.inspect}") if event.is_a?(MouseButtonEvent)
        @@listeners ||= []
        @@listeners.each { |listener|
            break if listener.input_event(event) == :handled
        }
    end

    # Set up getters for hash values.
    def self.hash_attr_reader(*names)
        names.each do |name|
            class_eval "def #{name}()  self[:#{name}];  end"
        end
    end

    # Every event has a type and a state.
    hash_attr_reader :type, :state
end

# Some specializations of Event follow.

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

    # Boolean event checkers for KeyboardEvent.

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
