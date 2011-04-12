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
                event, key_pressed_names, key_released_names = binding

                case event
                when Symbol; key, modifier = event, 0
                when Array;  key, modifier = event
                end

                # Add new event mappings.
                register_event(key_pressed_names, KeyPressed.new(key, modifier)) unless key_pressed_names.nil?
                register_event(key_released_names, KeyReleased.new(key, modifier)) unless key_released_names.nil?
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
        if @event_to_names.has_key?(event)
            $logger.warn "Event #{event} already triggers actions #{@event_to_names[event]}"
        else
            @event_to_names[event] = []
        end
        if name.is_a?(Array)
            @event_to_names[event] += name
        else
            @event_to_names[event] << name
        end
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
            names.each { |name| call_action(name) }
        end
    end
end
