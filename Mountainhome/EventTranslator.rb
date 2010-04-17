#!/usr/bin/ruby

# FIXME: The before and after Procs are a pretty brutal hack. We'll need a more robust
# priority-passing system soon.
# FIXME: KEY_PERCENT is broken; takes two tries to do anything.
# FIXME: uppercase_punc shouldn't be a global, but I'm not sure where to put it.

# Return a basic key_pressed event with no modifiers.
$uppercase_punc =
{
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

def basic_keypress(key)
    {:type => :keyboard, :state => :pressed, :key => key, :modifier => 0}
end

def shifted?(event)
    event[:modifier] == Keyboard.MOD_SHIFT or
    event[:modifier] == Keyboard.MOD_LSHIFT or
    event[:modifier] == Keyboard.MOD_RSHIFT
end

class EventTranslator
    def initialize
        @event_to_name  = {}
        @name_to_action = {}
        @before_mapping = nil
        @after_mapping  = nil
    end
    
    def set_action(name, &block)
        $logger.info("set_action: #{name} #{block.inspect}")
        @name_to_action[name] = block
    end

    def set_event(name, event)
        $logger.info("set_event: #{name} #{event}")
        @event_to_name[event] = name
    end

    def set_event_to_action(event, name, &block)
        @event_to_name[event] = name
        @name_to_action[name] = block
    end

    def default_before_action(&block)
        @before_mapping = block
    end

    def default_after_action(&block)
        @after_mapping = block
    end

    def translate(event)
        $logger.info("translate #{event.inspect}")
        if @before_mapping != nil
            status = @before_mapping.call(event)
            return status if status == :handled
        end

        unless (name = @event_to_name[event]).nil? or (action = @name_to_action[name]).nil?
            if action.is_a?(Proc)
                action.call
                return :handled
            end
        end

        if @after_mapping != nil
            return @after_mapping.call(event)
        end

        :unhandled
    end
end
