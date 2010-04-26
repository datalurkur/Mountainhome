module StateEventCreator
    def key_pressed(key, modifier)
        event = Event.key_pressed(key, modifier)
        key_pressed(event)
    end

    def key_released(key, modifier)
        event = Event.key_released(key, modifier)
        key_released(event)
    end

    def mouse_pressed(button, x, y)
        event = Event.mouse_pressed(button, x, y)
        mouse_pressed(event)
    end

    def mouse_released(button, x, y)
        event = Event.mouse_released(button, x, y)
        mouse_released(event)
    end

    def mouse_moved(absX, absY, relX, relY)
        event = Event.mouse_moved(absX, absY, relX, relY)
        mouse_moved(event)
    end
end