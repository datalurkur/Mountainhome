# This code takes events from the C-side and calls either self methods or input_event on a list of even takers.

module EventPasser
    attr_accessor :event_taker_list

    def send_events_to(*event_takers)
        @event_taker_list ||= []
        @event_taker_list += event_takers.select {|et| et.respond_to?(:input_event) }
        $logger.info("#{self.to_s} passes events to #{@event_taker_list.inspect}")
    end

    # Takes event from C.
    # In order, passes the event until respond_to? and returned :handled from:
    # 1) self.:id method
    # 2) self.input_event method
    # 3) @event_taker_list, set up via send_events_to
    def convert_event(id, *args)
        @event_taker_list ||= []
        event = Event.send(id, *args)
        # pass to e.g. key_pressed or mouse_moved if it exists
        if respond_to?(id)
            return if send(id, event) == :handled
        end
        [self, *@event_taker_list].each { |et|
            break if et.respond_to?(:input_event) and et.input_event(event) == :handled
        }
    end
end