# Event inherits from Hash, which brings in free object comparison based on hash entries which
# are the various getters/setters.

# Event also contains functionality for accepting events passed to it from C
# and passing them to registered listeners in order.

# Some useful keyboard constants.
MOUSEBUTTON_LEFT   = 1
MOUSEBUTTON_MIDDLE = 2
MOUSEBUTTON_RIGHT  = 3

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

# Translate modifier key-pressed into their equivalent modifier bits.
MODIFIERS = {
    Keyboard.KEY_RSHIFT => Keyboard.MOD_SHIFT,
    Keyboard.KEY_LSHIFT => Keyboard.MOD_SHIFT,
    Keyboard.KEY_RCTRL => Keyboard.MOD_CTRL,
    Keyboard.KEY_LCTRL => Keyboard.MOD_CTRL,
    Keyboard.KEY_RALT => Keyboard.MOD_ALT,
    Keyboard.KEY_LALT => Keyboard.MOD_ALT,
    Keyboard.KEY_RMETA => Keyboard.MOD_META,
    Keyboard.KEY_LMETA => Keyboard.MOD_META
}

class Event < Hash
    # Failure to remove listeners sometime after they've been added could lead
    # to bizarre situations like multiple UIManagers taking events. Don't do it!

    def self.add_listener(listener) add_listeners(listener); end
    def self.add_listeners(*listeners)
        @listeners ||= []
        @listeners += listeners.select {|et| et.respond_to?(:input_event) }
        $logger.info("Events passing to: #{@listeners.inject("") { |s, l| s + l.class.to_s + " "} }")
        listeners
    end

    # Insert a 'First Observer.'
    def self.add_priority_listener(listener)
        @listeners ||= []
        @listeners = [listener, *@listeners] if listener.respond_to?(:input_event) and !@listeners.include?(listener)
        $logger.info("Events prioritized to: #{listener}, @listener is #{@listeners.inject("") { |s, l| s + l.class.to_s + " "} }")
        @listeners
    end

    # Returns the list of removed listeners.
    def self.remove_listeners(*listeners)
        return nil if @listeners.nil?
        removed = []
        listeners.each do |listener|
            removed << listener unless @listeners.delete(listener).nil?
        end
        removed
    end

    def self.clear_listeners
        @listeners = []
    end

    # Send out new events releasing the keys with the old modifier status
    # and pressing the keys with the new modifier status.
    def self.release_on_mod_change(new_modifier)
        # Release all the currently pressed keys with the old modifier.
        @currently_pressed.each { |key| send_to_listeners(KeyReleased.new(key, @current_modifier)) }
        # Update the current modifier.
        @current_modifier = new_modifier
    end

    # Store the keys being currently pressed, and send corresponding press
    # and release events en masse when modifiers change.
    def self.save_state_change(event)
        # Nothing to be done for mouse events.
        return unless event.is_a?(KeyboardEvent)
        @currently_pressed ||= []
        @current_modifier ||= 0
        if event.modifier_key? # Event is a modifier key press or release.
            case event.state
            when :pressed
                # Add the new modifier to the current modifiers.
                release_on_mod_change(@current_modifier | MODIFIERS[event.key])
            when :released
                # Save all the modifiers except the one going away.
                release_on_mod_change(@current_modifier & ~MODIFIERS[event.key])
            end
        else # Event is a regular key press or release.
            case event.state
            when :pressed; @currently_pressed << event.key
            when :released; @currently_pressed.delete(event.key)
            end
        end
    end

    # Takes event passed from C.
    def self.receive_event(event)
        self.save_state_change(event)
        self.send_to_listeners(event)
    end

    # Passes the event to each @listener in order until respond_to? and
    # returned :handled from one of them.
    def self.send_to_listeners(event)
        $logger.info("sending event #{event.inspect}") if event.is_a?(MouseButtonEvent)
        @listeners ||= []
        @listeners.each { |listener|
            break if listener.input_event(event) == :handled
        }
    end

    # Set up getters for hash values.
    def self.hash_attr_reader(*names)
        names.each do |name|
            class_eval "def #{name}()  self[:#{name}];  end"
        end
    end

=begin
    # An abortive attempt to ignore DONTCARE modifiers
    def eql?(other)
        $logger.info "== #{self.inspect} #{other.inspect}"
        if self.respond_to?(:modifier) && self.modifier == Keyboard.MOD_DONTCARE
            # if self has a modifier and other does not, they don't ==
            return false unless other.respond_to?(:modifier)
            # Massage a new other to match self for the modifier part.
            other = other.dup
            other[:modifier] = Keyboard.MOD_DONTCARE
        elsif other.respond_to?(:modifier) && other.modifier == Keyboard.MOD_DONTCARE
            # if other has a modifier and self does not, they don't ==
            return false unless self.respond_to?(:modifier)
            # massage a new other to match self for the modifier part.
            other = other.dup
            other[:modifier] = self.modifier
        end
        result = super(other)
        $logger.info "result is #{result}"
        return result
    end
=end

    # Every event has a type and a state.
    hash_attr_reader :type, :state
end

# Some specializations of Event follow.

class KeyboardEvent < Event
    hash_attr_reader :key, :modifier

    def initialize(key = 0, modifier = 0)
        key = Keyboard.send(key) if key.kind_of?(Symbol)
        modifier = Keyboard.send(modifier) if modifier.kind_of?(Symbol)
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
            self[:modifier] = mod if self.modifier & mod != 0
        end
        self
    end

    def flatten
        self.dup.flatten!
    end

    # Change the key to the correct shifted character
    def ascii!
        return "" unless PRINTABLE.include?(self.key)
        if shift_held?
            # revise uppercase punctuation
            self[:key] = UPPERCASE_PUNCTUATION[self.key] if UPPERCASE_PUNCTUATION[self.key]

            # revise uppercase letters
            self[:key] -= 32 if (Keyboard.KEY_a..Keyboard.KEY_z).to_a.include?(self.key)
        end
        [self[:key]].pack('C')
    end

    def ascii
        self.dup.ascii!
    end

    # Boolean event checkers for KeyboardEvent.

    def printable?
        PRINTABLE.include?(self.key)
    end

    # Is this event a modifier key being pressed or released?
    def modifier_key?
        return self.key >= Keyboard.KEY_NUMLOCK
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
