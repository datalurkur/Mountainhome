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

        send(method_name, element) if element.visible?
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
    def slider_size;   20;             end
    def checkbox_size; 20;             end

    # =================
    # == Decorations ==
    # =================

    def add_border(element, color=border_color, size=1)
        element.add_renderables(
            create_border(0, 0, element.w, element.h, color, size)
        )
    end

    def create_border(x, y, w, h, color=border_color, size=1)
        [
            create_offset_rect_renderable(size, h+(size*2), x-size, y-size, color),
            create_offset_rect_renderable(size, h+(size*2), x+w, y-size, color),
            create_offset_rect_renderable(w, size, x, y+h, color),
            create_offset_rect_renderable(w, size, x, y-size, color)
        ]
    end

    def add_text_at(element, x, y, w, h, text, align=:center, font=default_font)
        text_width  = self.get_text_width(font, text)
        text_height = self.get_text_height(font)

        case align
        when :center
            text_x = (w - text_width) / 2.0 + x
        when :left
            text_x = 0
        when :right
            text_x = (w - text_width)
        end
        text_y = (h - text_height) / 2.0 + y

        create(Label, {:x => text_x, :y => text_y, :text => text, :depth => element.depth}, element)
    end

    def add_centered_text(element, text, font=default_font)
        add_text_at(element, 0, 0, element.w, element.h, text, :center, font)
    end
    def add_left_aligned_text(element, text, font=default_font)
        add_text_at(element, 0, 0, element.w, element.h, text, :left, font)
    end
    def add_right_aligned_text(element, text, font=default_font)
        add_text_at(element, 0, 0, element.w, element.h, text, :right, font)
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
    def prepare_uipane(element);    end
    def prepare_uimanager(element); end

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
    def prepare_textbody(element)
        single_text_height = self.get_text_height(default_font) + 2
        max_lines = (element.h / single_text_height).to_i

        snippets = self.split_text_at(default_font, element.text, element.w)[0...max_lines]

        snippets.each_with_index do |snippet, i|
            y_offset = element.h - (single_text_height * (i+1))
            add_text_at(element, 0, y_offset, element.w, single_text_height, snippet, :left)
        end
    end

    def prepare_image(element)
        element.add_renderable(
            create_rect_renderable(element.w, element.h, element.material)
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
        element.depth = UIElement::Depth[:override]
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

    def prepare_checkbox(element)
        element.w = element.h = checkbox_size

        renderables = []
        renderables << create_rect_renderable(element.w, element.h, element_color)
        renderables.concat(create_border(0,0,element.w,element.h))

        element.add_renderables(renderables)

        add_centered_text(element, "X") if element.checked
    end

    # Really, this is a horizontal slider; separate code will need to be written for a vertical one
    def prepare_slider(element)
        renderables = []

        # The slider background
        renderables << create_rect_renderable(element.w, element.h, element_color)

        # Ensure the location of the cursor in the slider bar has been initialized
        element.cursor_pos ||= [element.x, element.y]

        # Determine the value of the slider and the position of its renderable
        slider_width    = element.continuous ? self.slider_size : (element.w / element.values.size)
        slider_height   = element.h

        slider_extremes = [slider_width / 2.0, element.w - (slider_width / 2.0)]
        total_slider    = element.w - slider_width

        clamped_pos = [[element.cursor_pos[0].to_f, slider_extremes.min].max, slider_extremes.max].min
        position_ratio = (clamped_pos - slider_extremes.min) / total_slider

        if element.continuous
            value_range = element.values.max - element.values.min

            element.current_value = (value_range * position_ratio) + element.values.min
        else
            max_index = element.values.size - 1
            current_index = (max_index * position_ratio).round
            position_ratio = (current_index.to_f / max_index)

            element.current_value = element.values[current_index]
        end

        # The slider bar
        slider_pos = [(total_slider * position_ratio).to_i, 0]
        renderables << create_offset_rect_renderable(slider_width, slider_height, slider_pos[0], slider_pos[1], element_color)
        renderables.concat(create_border(slider_pos[0], slider_pos[1], slider_width, slider_height))

        # Add the slider extrema value labels
        add_left_aligned_text(element, element.values.min.to_s) if element.current_value != element.values.min
        add_right_aligned_text(element, element.values.max.to_s) if element.current_value != element.values.max
        add_text_at(element, slider_pos[0], slider_pos[1], slider_width, element.h, element.current_value.to_s)

        element.add_renderables(renderables)
    end

    def prepare_contextmenu(element)

        single_text_height = self.get_text_height(default_font) + 2
        dropdown_height = single_text_height * element.values.size

        if element.open?
            element.depth = UIElement::Depth[:focus]

            renderables = []

            # Add a background box for the dropdown
            renderables << create_offset_rect_renderable(element.w, dropdown_height, 0, -dropdown_height, element_color)

            # Add text labels for each dropdown value
            element.values.each_with_index do |value,i|
                y_offset = (single_text_height * i) - dropdown_height
                add_text_at(element, 0, y_offset, element.w, single_text_height, value.to_s, :left)
            end

            element.add_renderables(renderables)
        else
            element.depth = UIElement::Depth[:standard]
        end

        unless element.cursor_pos.nil?
            # Compute the new current value
            clamped_y = [[element.cursor_pos.y, 0].min, -dropdown_height].max
            clamped_x = [[element.cursor_pos.x, 0].max, element.w].min
            if clamped_y == element.cursor_pos.y && clamped_x == element.cursor_pos.x
                scaled_y  = (clamped_y / dropdown_height) + 1.0
                value_index = (scaled_y * (element.values.size-1)).round
                element.current_value = element.values[value_index]
            end
            element.cursor_pos = nil
        end
    end

    def prepare_dropdown(element)
        renderables = []
        renderables << create_rect_renderable(element.w, element.h, element_color)
        renderables.concat(create_border(0, 0, element.w, element.h))

        # Create the part of this menu that actually drops down (and use it to compute new values based on cursor position)
        prepare_contextmenu(element)

        # Add a label for the current value
        add_centered_text(element, element.current_value.to_s)

        element.add_renderables(renderables)
    end

    def prepare_console(element)
        single_text_height = self.get_text_height(default_font) + 2
        max_lines          = element.h / single_text_height

        renderables = []

        # Input Box
        renderables << create_rect_renderable(element.w, single_text_height*2, element_color)
        renderables.concat(create_border(0, 0, element.w, single_text_height*2))

        # Input Box Text
        add_text_at(element, 0, 0, element.w, single_text_height*2, element.text, :left) if element.text

        # History Box
        unless (max_lines <= 2)
            renderables << create_offset_rect_renderable(element.w, element.h - single_text_height*2, 0, single_text_height*2, element_color)

            # History Box Text
            current_line = 2
            element.history_buffer.reverse_each do |line|
                break if current_line > max_lines
                self.split_text_at(default_font, line, element.w).reverse.each do |snippet|
                    y_offset = single_text_height * current_line
                    add_text_at(element, 0, y_offset, element.w, single_text_height, snippet, :left)
                    current_line += 1
                end
            end
        end

        element.add_renderables(renderables)
    end
end
