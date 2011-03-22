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
        clean_element(element)
        send(method_name, element, manager)
    end

    def clean_element(element)
        clear_renderables(element)
        element.delete_dependents
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

    # =================
    # == Decorations ==
    # =================

    def add_border(element, color=border_color, size=1)
        # Left border
        add_offset_rect_renderable(element, size, element.h + size*2, -size, -size, color)
        # Right border
        add_offset_rect_renderable(element, size, element.h + size*2, element.w, -size, color)
        # Top border
        add_offset_rect_renderable(element, element.w, size, 0, element.h, color)
        # Bottom border
        add_offset_rect_renderable(element, element.w, size, 0, -size, color)
    end

    def add_centered_text(element, manager, text, font=default_font)
        text_width  = self.get_text_width(font, element.text)
        text_height = self.get_text_height(font)
        text_x = ((element.w - text_width)  / 2.0)
        text_y = ((element.h - text_height) / 2.0)

        manager.create(Label, {:parent => element, :dependent => true, :x => text_x, :y => text_y, :text => text})
    end

    # ==============
    # == Elements ==
    # ==============

    def prepare_debug(element, manager)
        add_rect_renderable(element, element.w, element.h, "blue")
    end

    def prepare_uielement(element, manager); end

    def prepare_label(element, manager)
        add_text_renderable(element, default_font, element.color || text_color, element.text)
    end
    def prepare_title(element, manager)
        add_text_renderable(element, title_font, element.color || title_color, element.text)
    end

    def prepare_inputfield(element, manager)
    end

    def prepare_mouse(element, manager)
        element.always_on_top
        add_offset_rect_renderable(element, 14, 21, 0, -21, "cursor.material")
    end

    def prepare_button(element, manager)
        add_rect_renderable(element, element.w, element.h, element_color)
        add_border(element, border_color, 2)
        add_centered_text(element, manager, element.text) if element.text
    end

    def prepare_invisiblebutton(element, manager)
        add_centered_text(element, manager, element.text) if element.text
    end

    def prepare_link(element, manager)
        text_width  = self.get_text_width(default_font, element.text)
        text_height = self.get_text_height(default_font)

        element.w = text_width
        element.h = text_height

        manager.create(Label, {:parent => element, :dependent => true, :color => link_color, :text => element.text})
    end

    def prepare_slider(element, manager)
        # Add the background of the slider (the element that delineates the slider boundaries)
        add_rect_renderable(element, element.w, element.h, element_color)

        # Compute an array of slider values
        slider_vals = case element.values
            when Array; element.values
            when Range; element.values.to_a
            else;       []
        end

        return if slider_vals.size <= 0

        # Compute the size of each section of the slider
        button_width  = element.w / slider_vals.size
        button_height = element.h

        # Add a button for each section of the slider
        slider_vals.each_with_index do |value, index|
            klass = (value == element.current_value) ? Button : InvisibleButton
            manager.create(klass, {
                :parent => element, :dependent => true,
                :text => value.to_s,
                :x => (index * button_width), :y => 0,
                :w => button_width, :h => button_height,
                :on_release => Proc.new { element.set(value) }
            })
        end
    end

    def prepare_grouping(element, manager)
        # Create the sub-elements
        sub_elements = element.sub_elements.collect do |sub_elem|
            if sub_elem.class == Hash
                klass = sub_elem[:element_class] || element.sub_element_class || UIElement
                attributes = sub_elem.merge(element.shared_attributes || {})

                create(element, manager, klass, attributes)
            else
                # If the sub element is not a hash, assume it's already been created or is nil (spacing)
                (sub_elem.parent = element) unless sub_elem.nil?
                sub_elem
            end
        end
        return if sub_elements.size <= 1

        # Arrange the sub-elements
        case element.type
        when :vertical
            total_space = element.h - sub_elements.last.h
            spacing = total_space / (sub_elements.size - 1)

            offset = 0
            sub_elements.reverse.each do |sub_elem|
                (sub_elem.y = offset) unless sub_elem.nil?
                offset = offset + spacing
            end
        else
            $logger.error "Grouping type #{element.type} not supported."
        end
    end
end
