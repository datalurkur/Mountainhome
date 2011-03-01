require 'UIElement'

class LookNFeel < MHLookNFeel
    def prepare_element(element, manager)
        class_name = element.class.to_s.downcase
        puts "[+] Preparing a #{class_name}"
        method_name = "prepare_#{class_name}"
        send(method_name, element, manager)
    end

    def method_missing(m, *args, &block)
        $logger.error "No method #{m} exists for the LookNFeel, cannot create #{args.first.class}"
    end

    def default_font
        "example.font"
    end

    def prepare_uielement(element, manager); end

    def prepare_label(element, manager)
        clear_renderables(element)
        add_text_renderable(element, default_font, element.text)
    end

    def prepare_inputfield(element, manager)
    end

    def prepare_mouse(element, manager)
        clear_renderables(element)
        add_rect_renderable(element, 14, 21, "cursor.material")
    end

    def prepare_button(element, manager)
        clear_renderables(element)
        add_rect_renderable(element, element.w, element.h, "white")

        text_width  = self.get_text_width(default_font, element.text)
        text_height = self.get_text_height(default_font)
        text_x = element.x + ((element.w - text_width)  / 2.0)
        text_y = element.y + ((element.h - text_height) / 2.0)

        manager.create(Label, {:parent => element, :x => text_x, :y => text_y, :text => element.text})
    end

    def prepare_slider(element, manager)
    end

    def prepare_grouping(element, manager)
    end
end
