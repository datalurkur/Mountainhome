require 'UIElement'

class Debug < UIElement; end

class LookNFeel < MHLookNFeel
    def initialize(manager)
        @manager = manager
    end

    # ==================
    # == Base Methods ==
    # ==================

    def prepare_element(element)
        class_name = element.class.to_s.downcase
        #$logger.info "[+] Preparing a #{class_name} with attributes\n#{element.inspect}"
        method_name = "prepare_#{class_name}"
        clean_element(element)
        send(method_name, element)
    end

    def clean_element(element)
        element.delete_renderables
        element.delete_dependents
    end

    def create(klass, args={}, dependee, &block)
        @manager.create(klass, args.merge(:dependent => true), dependee, &block)
    end

    def method_missing(m, *args, &block)
        $logger.error "No method #{m} exists for the LookNFeel, cannot create #{args.first.class}"
    end

    # ==============
    # == Defaults ==
    # ==============

    def default_font;  "example.font"; end
    def title_font;    "big.font";     end
    def element_color; "grey";         end
    def border_color;  "black";        end
    def text_color;    [0,0,0,0];      end
    def title_color;   [1,1,1,0];      end
    def link_color;    [1,0.2,0.2,0];  end
    def slider_size;   10;             end

    # =================
    # == Decorations ==
    # =================

    def add_border(element, color=border_color, size=1)
        element.add_renderables([
            create_offset_rect_renderable(size, element.h + size*2, -size, -size, color),
            create_offset_rect_renderable(size, element.h + size*2, element.w, -size, color),
            create_offset_rect_renderable(element.w, size, 0, element.h, color),
            create_offset_rect_renderable(element.w, size, 0, -size, color)
        ])
    end

    def add_centered_text(element, text, font=default_font)
        text_width  = self.get_text_width(font, element.text)
        text_height = self.get_text_height(font)
        text_x = ((element.w - text_width)  / 2.0)
        text_y = ((element.h - text_height) / 2.0)

        create(Label, {:x => text_x, :y => text_y, :text => text}, element)
    end

    def add_left_aligned_text(element, text, font=default_font)
        text_height = self.get_text_height(font)
        text_x = 0
        text_y = ((element.h - text_height) / 2.0)

        create(Label, {:x => text_x, :y => text_y, :text => text}, element)
    end

    # ==============
    # == Elements ==
    # ==============

    def prepare_debug(element)
        element.add_renderable(
            create_rect_renderable(element.w, element.h, "blue")
        )
    end

    def prepare_uielement(element); end
    def prepare_uipane(element); end

    def prepare_label(element)
        element.add_renderable(
            create_text_renderable(default_font, element.color || text_color, element.text)
        )
    end
    def prepare_title(element)
        element.add_renderable(
            create_text_renderable(title_font, element.color || title_color, element.text)
        )
    end

    def prepare_inputfield(element)
        element.add_renderable(
            create_rect_renderable(element.w, element.h, element_color)
        )
        add_border(element, border_color, 2)
        add_left_aligned_text(element, element.text) if element.text
    end

    def prepare_mouse(element)
        element.on_top = true
        element.add_renderable(
            create_offset_rect_renderable(14, 21, 0, -21, "cursor.material")
        )
    end

    def prepare_button(element)
        element.add_renderable(
            create_rect_renderable(element.w, element.h, element_color)
        )
        add_border(element, border_color, 2)
        add_centered_text(element, element.text) if element.text
    end

    def prepare_invisiblebutton(element)
        add_centered_text(element, element.text) if element.text
    end

    def prepare_link(element)
        text_width  = self.get_text_width(default_font, element.text)
        text_height = self.get_text_height(default_font)

        element.w = text_width
        element.h = text_height

        create(Label, {:color => link_color, :text => element.text}, element)
    end

    # Really, this is a horizontal slider; separate code will need to be written for a vertical one
    def prepare_slider(element)
        # Ensure the location of the cursor in the slider bar has been initialized
        element.cursor_pos ||= [element.x, element.y]

        # Determine the value of the slider and the position of its renderable
        position_ratio = element.cursor_pos[0].to_f / element.w
        if element.continuous
            element.current_value = (element.slider_values.max - element.slider_values.min) * position_ratio + element.slider_values.min
            slider_pos = [
                (element.cursor_pos[0] - (slider_size / 2.0)).to_i,
                0
            ]
        else
            max_index = element.slider_values.size - 1
            current_index = (max_index * position_ratio).round
            element.current_value = element.slider_values[current_index]
            slider_pos = [
                (((element.w / max_index) * current_index) - (slider_size / 2.0)).to_i,
                0
            ]
        end

        # Add the background of the slider (the element that delineates the slider boundaries)
        #  and the slider bar itself
        element.add_renderables([
            create_rect_renderable(element.w, element.h, element_color),
            create_offset_rect_renderable(slider_size, element.h, slider_pos[0], slider_pos[1], element_color)
        ])
    end
end
