#!/usr/bin/ruby

require 'Event'

class ActionPack
    def initialize(file = nil)
        @event_to_names  = {}
        @name_to_action = {}

        # Read a key-binding file.
        if file
            require file
            # Read a global name with the same name as the binding file read.
            file.constantize.events.each do |binding|
                name = binding.first
                event = binding.last
                # If it's a key, assume a KeyPressed event.
                event = KeyPressed.new(event) if event.kind_of?(Symbol)
                # Add a new event mapping.
                register_event(name, event)
            end
        end
    end

    def register_action(name, &block)
        $logger.info("Registering action #{name}, which is a #{block.class}")
        unless @name_to_action[name].nil?
            $logger.warn "Action name #{name} already registered!"
        end
        @name_to_action[name] = block
    end

    def register_event(name, event)
        $logger.info("Registering event #{event} to trigger action #{name}")
        @event_to_names[event] = [] unless @event_to_names.has_key?(event)
        @event_to_names[event] << name
    end

    def register_event_to_action(name, event, &block)
        register_event(name, event)
        register_action(name, &block)
    end

    def delete_mapping(name = nil, event = nil)
        @name_to_action.delete(name)
        @event_to_names[event].delete(name)
        @event_to_names.delete(event) if @event_to_names[event].empty?
    end

    def call_action(name)
        if (action = @name_to_action[name]) and
           (action.is_a?(Proc))
            action.call
            return :handled
        end
        :unhandled
    end

    def input_event(event)
        if names = @event_to_names[event]
            names.each { |name| $logger.info name.inspect; call_action(name) }
        end
    end
end
