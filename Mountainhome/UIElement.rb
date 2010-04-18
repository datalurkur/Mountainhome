class UIElement < MHUIElement
    attr_reader :name
    def initialize(name, manager, mat, font, text, args={})
        @update_proc = args[:update_proc] || nil
        @name = name

        super(name, manager, mat, font, text)
    end

    def update(elapsed)
        @update_proc.call if @update_proc
        each_child do |child|
            child.update(elapsed)
        end
    end

    def elements_at(x, y, d)
        collisions = []

        each_child do |child|
            child.elements_at(x, y, d+1).each { |subcoll| collisions << subcoll }
        end
        elem_x = self.x + self.x_offset
        elem_y = self.y + self.y_offset
        if @clickable and 
           (x >= elem_x) and (x <= (elem_x + self.w)) and
           (y >= elem_y) and (y <= (elem_y + self.h))
            return collisions << {:element => self, :d => d}
        else
            return collisions
        end
    end
end

class Clickable < UIElement
    attr_accessor :clickable

    def initialize(name, manager, mat, font, text, args={})
        @clickable = true
        @click_proc = args[:click_proc] || Proc.new { $logger.info "No click_proc specified for #{self.inspect}" }
        super(name, manager, mat, font, text, args)
    end

    def on_click
        @click_proc.call
    end
end

class InputField < UIElement
    def push_char(char, shifted = false)
        char = $uppercase_punc[char] if shifted and $uppercase_punc[char]
        self.text = (self.text + [char].pack("C"))
    end

    def pop_char
        self.text = self.text[0..-2]
    end
end

# This is used to create an invisible element whose dimensions don't matter
# We use this to group elements together for easy deletion
class Parent
    def self.build(name, manager)
        p_elem = manager.add_element("parent_#{name}", 0, 0, 0, 0, {:mat => ""})
        return p_elem
    end
end

class Button
    def self.build(name, manager, text, x, y, w, h, args={}, &block)
        t_dims = [manager.text_width(text)/2, manager.text_height/2]
        button_pos = [x - t_dims[0], y - t_dims[1]]
        button_offset = [x - (w/2) - button_pos[0], y - (h/2) - button_pos[1]]

        button = manager.add_element("button_#{name}", button_pos[0], button_pos[1], w, h,
                                     {:mat => "t_grey", :text => text, :parent => args[:parent],
                                      :element_type => Clickable, :click_proc => block})
        button.set_offset(button_offset[0], button_offset[1])
        button.set_border(2)

        return button
    end
end

class Title
    def self.build(name, manager, text, x, y, w, h, args={})
        t_dims = [manager.text_width(text)/2, manager.text_height/2]
        title_pos = [x - t_dims[0], y - t_dims[1]]
        title_offset = [x - (w/2) - title_pos[0], y - (h/2) - title_pos[1]]

        title = manager.add_element("title_#{name}", title_pos[0], title_pos[1], w, h,
                                    {:mat => "", :text => text, :parent => args[:parent]})
        title.set_offset(title_offset[0], title_offset[1])

        return title
    end
end

class Image
    def self.build(name, manager, mat, x, y, w, h, args={})
        i_pos = [x - (w/2), y - (h/2)]
        img = manager.add_element("image_#{name}", i_pos[0], i_pos[1], w, h,
                                  {:mat => mat, :parent => args[:parent]})
        return img
    end
end

class Console
    def initialize(manager, &block)
        @manager = manager

        @p_eval = block
        @toggled = false

        buffer_length = 15
        @history = Array.new(buffer_length, ">")

        # Create the menu elements
        @window = nil
        hist_upd = Proc.new { @window.text = @history.join("\n") }
        @window  = manager.add_element("console_window", 5, -30, manager.width-10, 220,
                                           {:mat => "t_grey", :update_proc => hist_upd})
        @window.set_offset(0,-205)

        @input_field = @manager.add_element("console_input",  5, -10, manager.width-10, 20,
                                           {:mat => "t_grey", :element_type => InputField})
        @input_field.set_offset(0,-5)
        @input_field.set_border(2)

        @elements = [@input_field, @window]
    end

    def input_event(event={})
        if @toggled
            case event[:key]
            when Keyboard.KEY_BACKQUOTE
                toggle
                return :handled
            when Keyboard.KEY_RETURN
                # Call the proc
                result = call(@input_field.text)
                # Place the command in history
                @history = [result, @input_field.text] + @history[0..-3]
                @input_field.text = ""
                return :handled
            when Keyboard.KEY_BACKSPACE
                @input_field.pop_char
                return :handled
            when Keyboard.KEY_a..Keyboard.KEY_z
                if shifted?(event)
                    event[:key] -= 32
                end
                @input_field.push_char(event[:key])
                return :handled
            # everything except KEY_PAUSE
            when Keyboard.KEY_BACKSPACE, Keyboard.KEY_TAB, Keyboard.KEY_CLEAR, Keyboard.KEY_RETURN, Keyboard.KEY_ESCAPE,
                 Keyboard.KEY_SPACE..Keyboard.KEY_DOLLAR,
                 Keyboard.KEY_AMPERSAND..Keyboard.KEY_AT,
                 Keyboard.KEY_LEFTBRACKET..Keyboard.KEY_UNDERSCORE
                @input_field.push_char(event[:key], shifted?(event))
                return :handled
            else
            end
        else 
        end
        return :unhandled
    end

    def toggled?
        @toggled
    end

    def toggle
        if @toggled
            @elements.each { |e| e.move_relative(0, -220) }
            @manager.active_element = nil
        else
            @elements.each { |e| e.move_relative(0, 220) }
            @manager.active_element = self
        end
        @toggled = !@toggled
    end

    def call(text)
        begin
            @p_eval.call(text)
        rescue Exception => e
            e.message
        end
    end
end
