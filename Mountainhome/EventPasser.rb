# This code takes events from the C-side and calls either self methods or input_event on a list of even takers.

module EventPasser
    def send_events_to(*event_takers)
        @event_takers ||= []
        @event_takers += event_takers.select {|et| et.respond_to?(:input_event) }
        $logger.info("#{self.to_s} passes events to #{@event_taker_list.inspect}")
    end

    # Takes event passed from C.
    # In order, passes the event until respond_to? and returned :handled from:
    # 1) self.input_event method
    # 2) @event_takers, set up via send_events_to
    def pass_event(event)
        @event_takers ||= []
        [self, *@event_takers].each { |et|
            break if et.respond_to?(:input_event) and et.input_event(event) == :handled
        }
    end
end