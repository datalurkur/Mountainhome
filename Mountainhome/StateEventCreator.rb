module StateEventCreator
    @@event_taker = self

    def events_pass_through(event_taker)
        @@event_taker = event_taker
    end

    def convert_event(id, *args)
        #$logger.info("convert_event #{id} #{args.inspect}")
        send(id, Event.send(id, *args)) if respond_to?(id)
    end
end
