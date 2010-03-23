class UIElement < MHUIElement
    def initialize(name, manager, mat, text, args={})
        @children = []
        @clickable = args[:clickable]
        super(name, manager, mat, text)
    end
    
    def set_dimensions(x,y,w,h)
        super(x,y,w,h)
    end
    
    def add_child(child)
        @children << child
        super(child)
    end

    def cull_child(child)
        if @children.include? child 
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
        if @clickable and 
           (x >= self.x) and (x <= (self.x + self.w)) and
           (y >= self.y) and (y <= (self.y + self.h))
            return collisions << {:element => self, :d => d}
        else
            return collisions
        end
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

class Console
    def initialize(manager)
        @input_field = manager.add_element(:name => "console_input", :mat => "white", :text => "",
                                           :x => 5, :y => -10, :w => 790, :h =>20, :element_type => InputField)
        @input_field.set_offset(0,-5)
        @input_field.set_border(2)
        @active = false
        @toggled = false
    end

    def input_event(args={})
        return :unhandled if !@toggled

        if args[:key] == Keyboard.KEY_RETURN
            if @active
                result = evaluate_expression(@input_field.text)
                @input_field.text = ""
            end
            @active = !@active
            return :handled
        end

        return :unhandled if !@active

        case args[:key]
        when Keyboard.KEY_BACKSPACE
        
            @input_field.pop_char
            return :handled
        #when ((Keyboard.KEY_a)..(Keyboard.KEY_z))
        else
            @input_field.push_char(args[:key].chr)
            return :handled
        end
        return :unhandled
    end

    def evaluate_expression(expr)
        $logger.info "Evaluating expression \"#{expr}\""
    end

    def toggle
        if @toggled
            hide
        else
            show
        end
    end

    def show
        @input_field.set_position(5, 5)
        @toggled = true
    end

    def hide
        @input_field.set_position(5, -10)
        @toggled = false
    end

    def teardown(manager)
        manager.kill_element(@input_field)
        @input_field = nil
    end
end
