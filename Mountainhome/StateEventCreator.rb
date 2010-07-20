module StateEventCreator
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
