class UIElement < MHUIElement
    attr_reader :name
    def initialize(name, manager, mat, font, text, args={})
        @children = []
        @update_proc = args[:update_proc] || nil
        @name = name

        super(name, manager, mat, font, text)
    end

    # CLEANUP - I don't want to maintain the list in C and in Ruby. Lets try to get this
    # JUST in ruby.
    def add_child(child)
        @children << child
        super(child)
    end

    # CLEANUP - A generic update proc? Seems a little strange...
    def update(elapsed)
        @update_proc.call if @update_proc
        @children.each { |c| c.update(elapsed) }
    end

    # CLEANUP - use select
    def cull_children(exceptions=[])
        cull = @children.select do |child|
            !exceptions.include?(child)
        end

        cull.each do |child|
            cull_child(child)
        end
    end

    # CLEANUP - do we even need to have a super here?
    def cull_child(child)
        if @children.include? child
            child.cull_children
            @children.delete(child) 
            super(child)
        else
            @children.each { |e_child| e_child.cull_child(child) }
        end
    end

    def elements_at(x, y, d)
        collisions = []
        @children.each do |child|
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
    def push_char(char)
        self.text = (self.text + char)
    end

    def pop_char
        self.text = self.text[0..-2]
    end
end

class SuperElement
    attr_accessor :elements

    def initialize(manager)
        @manager = manager
    end

    def teardown
        @elements.each { |elem| manager.kill_element(elem) }
        @elements = []
    end
end

class Button < SuperElement
    def initialize(name, manager, text, x, y, w, h, args={}, &block)
        super(manager)
        t_dims = [manager.text_width(text)/2, manager.text_height/2]
        button_pos = [x - t_dims[0], y - t_dims[1]]
        button_offset = [x - (w/2) - button_pos[0], y - (h/2) - button_pos[1]]

        @button = @manager.add_element("button_#{name}", button_pos[0], button_pos[1], w, h,
                                      {:mat => "t_grey", :text => text,
                                       :element_type => Clickable, :click_proc => block})
        @button.set_offset(button_offset[0], button_offset[1])

        @elements = [@button]
    end
end

class Console < SuperElement
    def initialize(manager, &block)
        super(manager)

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

    def input_event(args={})
        case args[:key]
        when Keyboard.KEY_BACKQUOTE
            return :unhandled
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
        else
            @input_field.push_char([args[:key]].pack("C"))
            return :handled
        end

        return :unhandled
    end

    def toggled?
        @toggled
    end

    def toggle
        if @toggled
            elements.each { |e| e.move_relative(0, -220) }
            @manager.active_element = nil
        else
            elements.each { |e| e.move_relative(0, 220) }
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
