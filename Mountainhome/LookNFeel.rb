require 'UIElement'

class LookNFeel < MHLookNFeel
    def prepare_element(element, manager)
        method_name = "prepare_#{element.class.to_s.downcase}"
        send(method_name, element, manager)
    end

    def method_missing(m, *args, &block)
        $logger.error "No method #{m} exists for the LookNFeel, cannot create #{args.first.class}"
    end

    def prepare_uielement(element, manager)
        $logger.info "Preparing UIElement"

        # AJEAN - TEMP CODE
        clear_renderables(element)
        add_rect_renderable(element, element.w, element.h, "white")
    end
end
