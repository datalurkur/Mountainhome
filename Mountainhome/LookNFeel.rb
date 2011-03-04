require 'UIElement'

class Debug < UIElement; end

class LookNFeel < MHLookNFeel
    # ==================
    # == Base Methods ==
    # ==================

    def prepare_element(element, manager)
        class_name = element.class.to_s.downcase
        $logger.info "[+] Preparing a #{class_name} with attributes\n#{element.inspect}"
        method_name = "prepare_#{class_name}"
        send(method_name, element, manager)
    end

    def method_missing(m, *args, &block)
        $logger.error "No method #{m} exists for the LookNFeel, cannot create #{args.first.class}"
    end

    # ==============
    # == Defaults ==
    # ==============

    def default_font;  "example.font"; end
    def element_color; "white";        end
    def border_color;  "black";        end
    def text_color;    [0,0,0,0];      end
    def title_color;   [1,1,1,0];      end
    def link_color;    [1,0.2,0.2,0];  end

    # =================
    # == Decorations ==
    # =================

    def add_border(element, color=border_color, size=1)
        # Left border
        add_offset_rect_renderable(element, size, element.h, -size, 0, color)
        # Right border
        add_offset_rect_renderable(element, size, element.h, element.w, 0, color)
        # Top border
        add_offset_rect_renderable(element, element.w + size*2, size, -size, element.h, color)
        # Bottom border
        add_offset_rect_renderable(element, element.w + size*2, size, -size, -size, color)
    end

    # ==============
    # == Elements ==
    # ==============

    def prepare_debug(element, manager)
        clear_renderables(element)
        add_rect_renderable(element, element.w, element.h, "blue")
    end

    def prepare_uielement(element, manager); end

    def prepare_label(element, manager)
        clear_renderables(element)
        add_text_renderable(element, default_font, element.color || text_color, element.text)
    end
    def prepare_title(element, manager)
        clear_renderables(element)
        # BUG - Setting both fonts concurrently causes a bad access during texture binding
        #add_text_renderable(element, "big.font", element.color || title_color, element.text)
    end

    def prepare_inputfield(element, manager)
    end

    def prepare_mouse(element, manager)
        clear_renderables(element)
        add_offset_rect_renderable(element, 14, 21, 0, -21, "cursor.material")
    end

    def prepare_button(element, manager)
        clear_renderables(element)
        add_rect_renderable(element, element.w, element.h, element_color)
        add_border(element, border_color, 2)

        text_width  = self.get_text_width(default_font, element.text)
        text_height = self.get_text_height(default_font)
        text_x = ((element.w - text_width)  / 2.0)
        text_y = ((element.h - text_height) / 2.0)

        manager.create(Label, {:parent => element,
                               :x => text_x, :y => text_y,
                               :text => element.text})
    end

    def prepare_link(element, manager)
        clear_renderables(element)

        text_width  = self.get_text_width(default_font, element.text)
        text_height = self.get_text_height(default_font)

        element.w = text_width
        element.h = text_height

        manager.create(Label, {:parent => element,
                               :color => link_color,
                               :text => element.text})
    end

    def prepare_slider(element, manager)
    end

    def prepare_grouping(element, manager)
    end
end
