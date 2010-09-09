module StateEventCreator
    @@event_taker = self

    def events_pass_through(event_taker)
        @@event_taker = event_taker
    end

    def convert_event(id, *args)
        $logger.info("convert_event #{id} #{args.inspect}")
        send(id, Event.send(id, *args)) if respond_to?(id)
    end

    def key_pressed0(key, modifier)
        event = Event.key_pressed(key, modifier)
        key_pressed(event) if respond_to?(:key_pressed)
    end

    def key_released0(key, modifier)
        event = Event.key_released(key, modifier)
        key_released(event) if respond_to?(:key_released)
    end

    def mouse_moved0(absX, absY, relX, relY)
        event = Event.mouse_moved(absX, absY, relX, relY)
        mouse_moved(event) if respond_to?(:mouse_moved)
    end

    def mouse_pressed0(button, x, y)
        event = Event.mouse_pressed(button, x, y)
        mouse_pressed(event) if respond_to?(:mouse_pressed)
    end

    def mouse_released0(button, x, y)
        event = Event.mouse_released(button, x, y)
        mouse_released(event) if respond_to?(:mouse_released)
    end
end
