class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
        @elements = []
        # Currently, calling super crashes things.  I'm not really sure why.
        #super(*args)
    end

    def teardown
        $logger.info "Tearing down UIManager"
    end

    def input_event(args={})
        case args[:type]
        when nil
            return "unhandled"
        when "mouse"
            $logger.info "UIManager received a mouseclick with args #{args.inspect}"
            return "handled"
        when "keyboard"
            return "handled"
        end
        return "unhandled"
    end

    def new_element(args={})
    end
end
