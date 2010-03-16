require 'UIElement'

class UIManager < MHUIManager
    def initialize(args={})
        $logger.info "Initializing UIManager with args #{args.inspect}"
        @elements = []
        super(args[:looknfeel])
    end

    def teardown
        $logger.info "Tearing down UIManager"
    end

    def input_event(args={})
        case args[:type]
        when :mouse
            $logger.info "UIManager received a mouseclick with args #{args.inspect}"
        when :keyboard
            return :handled
        end
        return :unhandled
    end

    def new_element(args={})
        uie = UIElement.new(args[:name], self, "white")
        uie.set_dimensions(args[:x],args[:y],args[:z], args[:w], args[:h], args[:d]||0)
    end

    # Find the topmost menu element at [x,y]
    def element_at(x, y)
        # Find all elements that exist at [x,y], keeping track of the one with the greatest depth from the root element
        topmost = nil
        @elements.each do |element|
            if (x >= element.x) and (x <= (element.x+element.width)) and
               (y >= element.y) and (y <= (element.y+element.height))
                (topmost = element) if ( (topmost == nil) or (element.depth > topmost.depth) )
            end
        end
        return topmost
    end
end
