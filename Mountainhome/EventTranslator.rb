#!/usr/bin/ruby

# Return a basic key_pressed event with no modifiers.
def basic_keypress(key)
    {:type => :keyboard, :state => :pressed, :key => key, :modifier => 0}
end

class EventTranslator
    def initialize
        @event_to_name  = {}
        @name_to_action = {}
        @before_mapping = nil
        @after_mapping  = nil
    end
    
    def set_action(name, &block)
        $logger.info("set_action: #{name} #{block.class}")
        @name_to_action = block
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

        $logger.info("translate after-before #{event.inspect}")
        if (name = @event_to_name[event]) != nil and (action = @name_to_action[name]) != nil
            if action.is_a?(Proc)
                action.call
                return :handled
            end
        end
        $logger.info("translate after-mid #{event.inspect}")

        if @after_mapping != nil
            return @after_mapping.call(event)
        end

        :unhandled
    end
end
