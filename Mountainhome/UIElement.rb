class UIElement < MHUIElement
    attr_reader :name, :parent
    attr_accessor :anchor_x, :anchor_y,
                  :update_proc, :manager,
                  :lay_x, :lay_y, :lay_w, :lay_h

    # Low-level setters and getters
    # =============================

    def anchor=(coords)
        #@anchor_x = coords[0]
        #@anchor_y = coords[1]
        @anchor_x, @anchor_y = coords
        self.align_element
    end

    def dims=(dims)
        #self.w = dims[0]
        #self.h = dims[1]
        self.w, self.h = dims
    end

    def ldims; [@lay_x, @lay_y]; end
    def ldims=(dims)
        if dims.size >= 2
            @lay_x = dims[0]
            @lay_y = dims[1]
        end
        if dims.size == 4
            @lay_w = dims[2]
            @lay_h = dims[3]
        end
    end

    def parent=(parent)
        @parent = parent
        @parent.add_child(self)
    end

    def snap; @snap || [nil,nil]; end
    def snap=(snap)
        if snap.class != Array
            @snap = [snap,nil]
        elsif snap.size < 2
            @snap = snap << nil
        else
            @snap = snap
        end
    end

    def text=(value)
        super(value)
        self.align_text
    end

    def text_align; @text_align || [nil,nil]; end
    def text_align=(text_align)
        if text_align.class != Array
            @text_align = [text_align,nil]
        elsif text_align.size < 2
            @text_align = text_align << nil
        else
            @text_align = text_align
        end
    end

    # High-level element modifier methods
    # ===================================

    def update(elapsed)
        @update_proc.call unless @update_proc.nil?
        each_child { |child| child.update(elapsed) }
    end

    def lmove(dims)
        self.ldims = [self.ldims[0] + dims[0], self.ldims[1] + dims[1]]
        self.compute_dimensions
    end

    # Methods for computing and recomputing position and size
    # =======================================================

    def compute_anchors(parent_dims, lay_dims)
        # Negative lay indices indicate that this lay line starts from the extreme end rather than the near end
        self.lay_x += lay_dims[2]+1 if self.lay_x < 0
        self.lay_y += lay_dims[2]+1 if self.lay_y < 0

        # Discover the focus of this element using lay-lines
        self.anchor_x = (self.lay_x * lay_dims[0]) + parent_dims[0]
        self.anchor_y = (self.lay_y * lay_dims[1]) + parent_dims[1]
    end

    def align_element
        # Align the element properly
        self.x = self.anchor_x
        self.y = self.anchor_y

        if    snap[0] == :right;  self.x -=  self.w
        elsif snap[0] == :center; self.x -= (self.w / 2.0)
        end

        if    snap[1] == :top;    self.y -=  self.h
        elsif snap[1] == :center; self.y -= (self.h / 2.0)
        end
    end

    def align_text
        case self.text_align[0]
        when :right
            self.x_offset = self.w - self.text_width
        when :left
            self.x_offset = 0
        else
            self.x_offset = (self.w - self.text_width)/2
        end

        case self.text_align[1]
        when :top
            self.y_offset = self.h - self.text_height
        when :bottom
            self.y_offset = 0
        else
            self.y_offset = (self.h - self.text_height)/2
        end
    end

    def compute_dimensions
        # Get the local boundaries and dimensions
        parent_dims = if self.parent.nil?
            [0,                  0,
             self.manager.width, self.manager.height]
        else
            [self.parent.x,      self.parent.y,
             self.parent.w,      self.parent.h]
        end
        max_lay = self.manager.looknfeel.lay_divisions.to_f
        x_lay_size = parent_dims[2] / max_lay
        y_lay_size = parent_dims[3] / max_lay

        # Compute the element's anchor point
        if self.lay_x.nil? || self.lay_y.nil?
            self.anchor_x ||= 0
            self.anchor_y ||= 0
        else
            compute_anchors(parent_dims, [x_lay_size, y_lay_size, max_lay]) unless self.lay_x.nil? || self.lay_y.nil?
        end

        # Size the element in the pixel-domain
        self.w = self.lay_w * x_lay_size unless self.lay_w.nil?
        self.h = self.lay_h * y_lay_size unless self.lay_h.nil?
        self.w ||= 0
        self.h ||= 0

        # Align the element properly around the anchor point
        align_element

        align_text unless self.text.nil? || self.text == ""
        each_child { |child| child.compute_dimensions }
    end

    def elements_at(x, y, d)
        collisions = []

        each_child do |child|
            child.elements_at(x, y, d+1).each { |subcoll| collisions << subcoll }
        end
        elem_x = self.x
        elem_y = self.y
        if @clickable &&
           (x >= elem_x) && (x <= (elem_x + self.w)) &&
           (y >= elem_y) && (y <= (elem_y + self.h))
            return collisions << {:element => self, :d => d}
        else
            return collisions
        end
    end
end

class Pane < UIElement; end

class Text < UIElement
    def initialize(*args)
        super(*args)

        @snap       = [:center, :center]
        @text_align = [:left, :center]
        self.ldims  = [0,0]
    end
end

class Title < Text; end

class Box < UIElement
    def initialize(*args)
        super(*args)

        self.set_border(1)
    end
end

class Clickable < UIElement
    attr_accessor :click_proc

    def initialize(*args, &block)
        super(*args)

        @clickable  = true
        @click_proc = block if block_given?
    end

    def on_click(&block)
        if @click_proc.nil?
            $logger.info "No click proc specified for #{self.inspect}"
        else
            @click_proc.call { yield if block }
        end
    end
end

class Button < Clickable
    def initialize(*args, &block)
        super(*args) { |*args| block.call(*args) if block_given? }

        set_border(1)
    end
end

class InputField < UIElement
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

class CheckBox < Clickable
    attr_accessor :tracker

    def initialize(*args)
        @snap       = [:left,   :center]
        @text_align = [:center, :center]
        @state      = false

        super(*args)

        self.text = " "
        max_size  = [self.text_width, self.text_height].max * 1.5
        self.w    = max_size
        self.h    = max_size

        set_border(1)
    end

    def on_click
        @state = !@state
        self.text = @state ? "X" : ""
        if @tracker.nil?
            $logger.info "No tracker specified for checkbox #{name}"
        else
            @tracker.call(@state)
        end
    end
end

class Mouse < UIElement
    def initialize(*args)
        super(*args)

        set_dimensions(0,0,14,21)
        @pointer_offset = [0,-21]
        always_on_top
    end

    def x; self.anchor_x; end
    def x=(val)
        self.anchor_x = val
        super(val+@pointer_offset[0])
    end

    def y; self.anchor_y; end
    def y=(val)
        self.anchor_y = val
        super(val+@pointer_offset[1])
    end
end

class Slider < Clickable
    def initialize(*args, &block)
        super(*args)

        @tracker = block_given? ? block : Proc.new { $logger.info "The slider, it does nothing!" }
        @manager = args[1]
        @snap    = [:left, :center]

        @moving  = false

        full = @manager.looknfeel.lay_divisions
        half = full / 2
        @slider_line   = @manager.create(Box, {:parent=>self, :ldims=>[0,half,full,0], :snap=>[:left,  :center]})
        @slider_handle = @manager.create(Box, {:parent=>self, :ldims=>[0,half,8,full], :snap=>[:center,:center],
                                               :text=>"", :text_align=>[:center,:center]})
    end

    def default=(value)
        @value = value
        self.update_position
    end

    def range=(range); @range=range; end
    def range; @range || [0.0,1.0]; end

    def get_value_at_position(position)
        # Interpolate to find the new value of the slider
        (((position - self.x) / self.w) * (@range.last - @range.first)) + @range.first
    end

    def get_position_at_value(value)
        self.x + (self.w * ((value - @range.first) / (@range.last - @range.first)))
    end

    def compute_dimensions
        super
        self.update_position
    end

    def update(elapsed)
        if @moving
            slider_min = self.x
            slider_max = self.x + self.w

            # Get the position of the mouse in slider-space
            position = @source.call
            position = [[position, slider_min].max, slider_max].min.to_f

            # Get the new value
            @value = get_value_at_position(position)

            # Provide the new value to the tracker
            @tracker.call(@value)

            # Move the handle
            self.update_position
        end
    end

    def update_position
        position = get_position_at_value(@value)
        @slider_handle.anchor_x = position
        @slider_handle.align_element
        @slider_handle.text = @value.to_s
    end

    def on_click(&block)
        @moving = true
        @source = block_given? ? block : Proc.new { "The slider, it can do nothing!" }
    end

    def on_release
        @moving = false
        @source = nil
    end
end

class TickSlider < Slider
    def values; @valuearr && @valuearr.size > 0 ? @valuearr : [0,1]; end
    def values=(array)
        @valuearr = array
        self.update_position
    end

    def get_value_at_position(position)
        values = self.values
        index  = ((values.size-1) * ((position - self.x) / self.w)).to_i
        values[index]
    end

    def get_position_at_value(value)
        values   = self.values
        index    = values.index(value).to_f
        position = self.x + (self.w * (index / (values.size-1)))
    end
end

class InfoDialog < Box
    def initialize(*args)
        super(*args)

        manager = args[1]
        half = manager.looknfeel.lay_divisions / 2
        @textbox = manager.args[1].create(Text, {:parent=>self, :ldims=>[half,-1]})
        ok_button = manager.create(Button, {:parent=>self, :ldims=>[half,1], :snap=>[:center,:center], :text=>"OK"}) {
            manager.kill_element(self)
            manager.focus_override = nil
        }

        # This prevents users from ignoring the dialog
        manager.focus_override = [ok_button]
    end

    def text=(value)
        @textbox.text = value
    end
end
=begin
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
=end

# DropDown is a button that the user clicks on which generates a drop-down menu of choices.
# The drop-down menu is generated and handled by the list_proc, which should take the DropDown as a parameter
class DropDown < Button
    def initialize(*args, &block)
        super(*args)

        @manager = args[1]
        @state   = :closed
        @snap    = [:left, :center]

        @click_proc  = Proc.new { self.toggle }
        @select_proc = block_given? ? block : Proc.new { "The dropdown, it does nothing!" }
    end

    def default=(value); select(value); end
    def list=(value, &block); @list = block_given? ? block : value; end
    def select(value); self.text = value; @select_proc.call(value); end

    def toggle
        if @state == :open
            @state = :closed

            # Collapse the list by destroying the list elements
            self.cull_children
        else
            @state = :open

            # Get the list either from the proc given or the array given
            list = @list.class==Proc ? @list.call : @list

            # Create the list of items
            list.each_with_index do |item,index|
                item_x = self.anchor_x
                item_y = self.anchor_y - ((index + 1) * self.h)
                @manager.create(Clickable, {:parent=>self, :snap=>[:left,:center], :text=>item,
                                         :anchor=>[item_x, item_y], :dims=>[self.w,self.h]}) { 
                    self.select(item)
                    self.toggle
                }
            end
        end
    end
end

class Console < Pane
    def initialize(*args, &block)
        super(*args)

        @manager       = args[1]
        @toggled       = false
        @buffer_length = 20
        @p_eval        = block_given? ? block : Proc.new { $logger.info "The console, it does nothing!" }

        full = @manager.looknfeel.lay_divisions
        half = full / 2

        self.ldims = [half,-3,full,half-2]
        self.snap  = [:center,:bottom]

        @input_field = @manager.create(InputField, {:parent=>self,:ldims=>[0,3,full,2],:text_align=>[:left,:center],:snap=>[:left,:top]})
        clear_history
        update_proc    = Proc.new { @history_panel.text = @history[0..@buffer_length].reverse.join("\n"); @history_panel.align_text }
        @history_panel = @manager.create(Pane, {:parent=>self,:ldims=>[0,3,full,full-3],:text_align=>[:left,:top],:snap=>[:left,:bottom],:update_proc=>update_proc})
    end

    def clear_history
        # stores cmds and cmd results
        @history = Array.new(@buffer_length, ">")
        # stores cmds for retrieval
        @cmd_history = []
        @cmd_placement = nil

        @input_field.text = ""
    end

    def input_event(event)
        if @toggled
            case event.key
            when Keyboard.KEY_BACKQUOTE
                toggle
                return :handled
            when Keyboard.KEY_RETURN
                clear_history if @input_field.text == "clear"
                return :handled if @input_field.text.length == 0

                # Call the proc
                result = call(@input_field.text)
                # Place the command + results in history
                @history = [result, @input_field.text] + @history
                # Place command after beginning of command history
                @cmd_history.insert(0, @input_field.text)
                @cmd_placement = nil
                @input_field.text = ""
                return :handled
            when Keyboard.KEY_BACKSPACE
                @input_field.pop_char
                return :handled
            when Keyboard.KEY_DOWN
                # go 'back' in cmd history
                return :handled if @cmd_history.empty?
                # save 'latest' cmd contents, since this is not yet in @cmd_history
                if @cmd_placement.nil?
                    @cmd_placement = 0
                    @original_text = @input_field.text
                else
                    # don't go out of bounds
                    @cmd_placement += 1 unless @cmd_placement + 1 == @cmd_history.size
                end
                @input_field.text = @cmd_history[@cmd_placement]
                return :handled
            when Keyboard.KEY_UP
                # go 'forward' in cmd history
                return :handled if @cmd_history.empty? || @cmd_placement.nil?
                # load 'latest' cmd contents
                if @cmd_placement == 0
                    @cmd_placement = nil
                    @input_field.text = @original_text
                else
                    @cmd_placement -= 1
                    @input_field.text = @cmd_history[@cmd_placement]
                end
                return :handled
            else
                if event.printable?
                    @input_field.push_char(event)
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
        move_distance = self.lay_h-3
        if @toggled
            self.lmove([0,move_distance])
            @manager.active_element = nil
        else
            self.lmove([0,-move_distance])
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
