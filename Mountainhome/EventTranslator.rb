#!/usr/bin/ruby

# FIXME: The before and after Procs are a pretty brutal hack. We'll need a more robust
# priority-passing system soon.

require 'Event'

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
#        $logger.info("translate #{event.inspect}")

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
