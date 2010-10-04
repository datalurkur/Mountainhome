# FIXME: How to set up @event_taker_list properly??

# This code takes events from the C-side and calls either self methods or input_event on a list of even takers.

module EventPasser
    def send_events_to(*event_takers)
        @event_taker_list ||= event_takers.select {|et| et.respond_to?(:input_event) }
        $logger.info("Here we are in send_events_to: etl is #{@event_taker_list.inspect}")
    end

    # Takes event from C.
    # In order, passes the event until respond_to? and returned :handled from:
    # 1) self.id method
    # 2) self.input_event method
    # 3) @event_taker_list via send_events_to
    def convert_event(id, *args)
        @event_taker_list ||= []
        #$logger.info("convert_event #{id} #{args.inspect}")
        event = Event.send(id, *args)
        # pass to e.g. key_pressed or mouse_moved if it exists
        if respond_to?(id)
            send(id, event)
        else
            [self, *@event_taker_list].each { |et|
                break if et.respond_to?(:input_event) and et.input_event(event) == :handled
            }
        end
    end
end