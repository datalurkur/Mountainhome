class UIElement < MHUIElement
    attr_reader :name
    def initialize(name, manager, mat, font, text, args={})
        @update_proc    = args[:update_proc]

        # Pass the parameters to the C Object
        super(name, manager, mat, font, text)

        if args[:parent]
            args[:parent].add_child(self)
        end
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

    def initialize(name, manager, mat, font, text, args={}, &block)
        @clickable = true
        @click_proc = block || Proc.new { $logger.info "No click_proc specified for #{self.inspect}" }
        super(name, manager, mat, font, text, args)
    end

    def on_click(&block)
        @click_proc.call { yield if block }
    end
end

class InputField < UIElement
    def initialize(name, manager, x, y, w, h, args={})
        super(name, manager, "white", "", "", args)
        set_dimensions(x, y, w, h)
    end

    def push_char(event)
        self.text = (self.text + [event.convert_shift.key].pack("C"))
        # Seems to follow a printf format, where "%%" resolves to '%'
        if event.key == Keyboard.KEY_PERCENT
            self.text = (self.text + [event.convert_shift.key].pack("C"))
        end
    end

    def pop_char
        self.text = self.text[0..-2]
    end
end

# This is used to create an invisible element whose dimensions don't matter
# We use this to group elements together for easy deletion
class Invisible < UIElement
    def initialize(name, manager, args={})
        super("invisible_#{name}", manager, "", "", "", args)
    end
end

class Selectable < Clickable
    def initialize(name, manager, text, x, y, w, h, args={}, &block)
        super("selectable_#{name}", manager, args[:mat] || "transparent_grey.material", "", text, args) { yield if block }

        set_dimensions(x, y, w, h)
    end
end

class Button < Selectable
    def initialize(name, manager, text, x, y, w, h, args={}, &block)
        t_dims = [manager.text_width(text)/2, manager.text_height/2]
        button_pos = [x - t_dims[0], y - t_dims[1]]
        button_offset = [x - (w/2) - button_pos[0], y - (h/2) - button_pos[1]]

        super(name, manager, text, button_pos[0], button_pos[1], w, h, args) { yield if block }

        set_offset(button_offset[0], button_offset[1])
        set_border(2)
    end
end

class Pane < UIElement
    def initialize(name, manager, x, y, w, h, args={})
        super("pane_#{name}", manager, args[:mat] || "transparent_grey.material", "", "", args)

        set_dimensions(x,y,w,h)
    end
end

class CheckBox < Clickable
    def initialize(name, manager, def_value, x, y, args={}, &block)
        w = args[:width]  || 2*manager.text_width("X")
        h = args[:height] || 2*manager.text_height
        @state = def_value
        @tracker = block || Proc.new { $logger.info "No tracker specified for checkbox #{name}" }

        super("checkbox_#{name}", manager, "transparent_grey.material", "", "", args)

        set_dimensions(x-(w/2), y-(h/2), w, h)
        set_border(1)

        @state_text = Text.new("checkbox_#{name}_state", manager, @state ? "X" : "", x-(w/4), y, {:parent => self})
    end

    def on_click
        @state = !@state
        @state_text.text = @state ? "X" : ""
        @tracker.call(@state)
    end
end

class Text < UIElement
    def initialize(name, manager, text, x, y, args={})
        t_dims = [manager.text_width(text), manager.text_height]
        t_pos = [x - (t_dims[0]/2), y - (t_dims[1]/2)]

        super("title_#{name}", manager, "", args[:font] || "", text, args)

        set_dimensions(t_pos[0], t_pos[1], t_dims[0], t_dims[1])
    end
end

class Image < UIElement
    def initialize(name, manager, mat, x, y, w, h, args={})
        i_pos = [x - (w/2), y - (h/2)]

        super("image_#{name}", manager, mat, "", "", args)

        set_dimensions(i_pos[0], i_pos[1], w, h)
    end
end

class Mouse < UIElement
    def initialize(manager, args={})
        super("mouse", manager, "cursor.material", "", "")

        set_dimensions(0,0,14,21)
        set_offset(0,-21)
        always_on_top
    end
end

# Slider tracks a single value from 0.0 to 1.0, passing it to the block as an argument when it changes
class Slider < Selectable
    def initialize(name, manager, def_value, x, y, w, h, args = {}, &block)
        @value  = def_value || 0.5
        @moving = false
        tab_width = 8

        # This proc is used to obtain whatever value is using to modify the position of the slider
        @source = Proc.new { $logger.info "No data source specified for slider #{name}" }

        # This proc is used to pass the slider's value back to whatever object cares
        @tracker = block || Proc.new { nil }

        s_loc = [x-(w/2), y-(h/2)]
        super("slider_#{name}", manager, "", s_loc[0], s_loc[1], w, h, args.merge!(:mat => ""))

        Pane.new("sliderbar_#{name}", manager, s_loc[0], s_loc[1]+(h/2)-(tab_width/4), w, tab_width/2, {:parent => self})
        @tab = Pane.new("sliderpane_#{name}", manager, s_loc[0]+((w - tab_width)*@value), s_loc[1], tab_width, h, {:parent => self})
        @tab.set_border(2)
    end

    def update(elapsed)
        if @moving
            # Obtain the source value and keep the slider within its boundaries
            @tab.x = [[@source.call, slider_min].max, slider_max].min
            @value = (slider_position).to_f / slider_width

            # Pass the new slider value back
            @tracker.call(@value)
        end
    end

    def slider_max
        self.x + self.w - @tab.w
    end

    def slider_min
        self.x
    end

    def slider_width
        self.w - @tab.w
    end

    def slider_position
        @tab.x - self.x
    end

    def on_click(&block)
        @moving  = true
        (@source = block) if block
    end

    def on_release
        @moving = false
        @source = Proc.new { $logger.info "No tracker specified for slider #{name}" }
    end
end

class TickSlider < Slider
    def initialize(name, manager, def_value, values, x, y, w, h, args = {}, &block)
        @values = values
        def_value = @values.index(def_value).to_f / (@values.size - 1.0)

        super(name, manager, def_value, x, y, w, h, args) { yield @value.to_s }

        @values.each_with_index do |val,index|
            Text.new("#{val}_slider_#{name}_label", manager, val.to_s, slider_min + (value_width*index), y-h, {:parent => self})
        end
    end

    def value_width
        slider_width.to_f / (@values.size - 1)
    end

    def update(elapsed)
        if @moving
            # First, compute the value along the slider like normal
            linear_x     = [[@source.call, slider_min].max, slider_max].min
            linear_value = (linear_x - slider_min).to_f / slider_width

            # Snap the value to one of the values in the list (lerp, essentially)
            max_index = @values.size - 1
            # Do a lookup to see which value the user is hoving closest to
            value_index = ((linear_value * max_index) + 0.5).to_i

            # Set the slider location accordingly
            @tab.x = (value_index * value_width) + slider_min
            # Get the value and do the callback
            @value = @values[value_index]
            @tracker.call
        end
    end
end

class InfoDialog < Pane
    def initialize(name, manager, message, x, y, w, h, args={})
        super(name, manager, x-(w/2), y-(h/2), w, h, args.merge!(:mat => "grey"))
        set_border(2)

        Text.new("dialog_#{name}", manager, message, x, y+(h/4), {:parent => self})

        # This prevents users from ignoring the dialog
        ok_button = Button.new("dialog_#{name}_ok", manager, "OK", x, y-(h/4), 40, 20, {:parent => self}) do
            manager.kill_element(self)
            manager.focus_override = nil
        end

        manager.focus_override = [ok_button]
    end
end

class InputDialog < Pane
    def initialize(name, manager, message, x, y, w, h, args={}, &block)
        @callback = block || Proc.new { $logger.info "No callback specified for InputDialog #{name}" }
        @manager = manager

        super(name, manager, x-(w/2), y-(h/2), w, h, args.merge!(:mat => "grey"))
        set_border(2)

        manager.active_element = self

        Text.new("input_dialog_#{name}", manager, message, x, y+(h/4), {:parent => self})
        @field_data = InputField.new(name, manager, x-(w/2)+20, y, w-40, 20, {:parent => self})

        ok_button = Button.new("OK", manager, "Save", x-(w/4), y-(h/4), 40, 20, {:parent => self}) do
            @callback.call(@field_data.text)
            teardown
        end
        cancel_button = Button.new("Cancel", manager, "Cancel", x+(w/4), y-(h/4), 40, 20, {:parent => self}) do
            teardown
        end

        manager.focus_override = [ok_button, cancel_button]
    end

    def teardown
        @manager.kill_element(self)
        @manager.focus_override = nil
        @manager.active_element = nil
    end

    def input_event(event)
        $logger.info "Input dialog receives event #{event.inspect}"
        case event[:key]
        when Keyboard.KEY_a..Keyboard.KEY_z
            @field_data.push_char(event)
        when Keyboard.KEY_BACKSPACE
            @field_data.pop_char
        when Keyboard.KEY_RETURN
            @callback.call(@field_data.text)
            teardown
        end
    end
end

class ListSelection < Pane
    attr_accessor :list, :label

    def initialize(name, manager, label, list, x, y, w, h, args={}, &block)
        super(name, manager, x, y, w, h, args)
        set_border(2)

        @label = Text.new("#{name}_selector_label", manager, label, x+(manager.text_width(label)/2), y+h+manager.text_height, {:parent => self})
        @elems = Invisible.new("#{name}_selector_grouper", manager, {:parent => self})

        @manager = manager
        @tracker = block || Proc.new { $logger.info "No tracker specified for SelectionList #{name}" }

        @list = list
        @element_height = @manager.text_height * 2

        gen_children
    end

    def select(index)
        # Kill off the selections
        @elems.cull_children
        # Defer to the tracker to change the element list and label appropriately
        @tracker.call(index)
        # Regenerate the children and the label accordingly
        gen_children
        @label.x = (self.x + (@manager.text_width(@label.text)/2))
    end

    def gen_children
        @list.each_with_index do |item,index|
            elem_shift = (1+index) * @element_height
            break if elem_shift > h

            this_item = Selectable.new("name_item_#{item}", @manager, item, self.x, self.h+self.y-elem_shift, self.w, @element_height, {:parent => @elems}) { select(index) }
        end
    end
end

# DropDown is a button that the user clicks on which generates a drop-down menu of choices.
# The drop-down menu is generated and handled by the list_proc, which should take the DropDown as a parameter
class DropDown < Selectable
    def initialize(name, manager, def_value, x, y, w, h, args={}, &block)
        @state     = :closed
        @list_proc = block || Proc.new { $logger.info "No list proc specified for DropDown element #{name}" }

        d_dims = [x-(w/2.0), y-(h/2.0)]

        super("dropdown_#{name}", manager, def_value, d_dims[0], d_dims[1], w, h, args) { toggle }

        set_border(2)
    end

    def selected(item)
        self.text = item
        toggle
    end

    def toggle
        case @state
        when :closed
            @state = :open
            @list_proc.call(self)
        when :open
            @state = :closed
            self.cull_children
        end
    end
end

class Console < InputField
    def initialize(manager, args={}, &block)
        @manager = manager

        @p_eval = block
        @toggled = false

        @buffer_length = 15
        # initialize history
        clear_history

        # This element will be the input field, with the history buffer as its child
        super("console", manager, 5, -10, manager.width-10, 20, args)

        set_offset(0,-5)
        set_border(2)

        @window = nil
        hist_upd = Proc.new { @window.text = @history[0..@buffer_length].join("\n") }
        @window = Pane.new("console_history", manager, 5, -30, manager.width-10, 220, {:update_proc => hist_upd})
        @window.set_offset(0,-205)
        add_child(@window)
    end

    def clear_history
        # stores cmds and cmd results
        @history = Array.new(@buffer_length, ">")
        # stores cmds for retrieval
        @cmd_history = []
        @cmd_placement = nil

        self.text = ""
    end

    def input_event(event)
        if @toggled
            case event.key
            when Keyboard.KEY_BACKQUOTE
                toggle
                return :handled
            when Keyboard.KEY_RETURN
                clear_history if self.text == "clear"
                return :handled if self.text.length == 0

                # Call the proc
                result = call(self.text)
                # Place the command + results in history
                @history = [result, self.text, @window.text] + @history
                # Place command after beginning of command history
                @cmd_history.insert(0, self.text)
                @cmd_placement = nil
                self.text = ""
                return :handled
            when Keyboard.KEY_BACKSPACE
                pop_char
                return :handled
            when Keyboard.KEY_DOWN
                # go 'back' in cmd history
                return :handled if @cmd_history.empty?
                # save 'latest' cmd contents, since this is not yet in @cmd_history
                if @cmd_placement.nil?
                    @cmd_placement = 0
                    @original_text = self.text
                else
                    # don't go out of bounds
                    @cmd_placement += 1 unless @cmd_placement + 1 == @cmd_history.size
                end
                self.text = @cmd_history[@cmd_placement]
                return :handled
            when Keyboard.KEY_UP
                # go 'forward' in cmd history
                return :handled if @cmd_history.empty? || @cmd_placement.nil?
                # load 'latest' cmd contents
                if @cmd_placement == 0
                    @cmd_placement = nil
                    self.text = @original_text
                else
                    @cmd_placement -= 1
                    self.text = @cmd_history[@cmd_placement]
                end
                return :handled
            else
                if event.printable?
                    push_char(event)
                    return :handled
                end
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
            [self, @window].each { |e| e.move_relative(0, -220) }
            @manager.active_element = nil
        else
            [self, @window].each { |e| e.move_relative(0, 220) }
            @manager.active_element = self
        end
        @toggled = !@toggled
    end

    def call(local_text)
        begin
            @p_eval.call(local_text)
        rescue Exception => e
            e.message
        end
    end
end
