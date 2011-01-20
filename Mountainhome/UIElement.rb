class UIElement < MHUIElement
    attr_reader :name, :parent
    attr_accessor :anchor_x, :anchor_y,
                  :update_proc, :manager,
                  :lay_x, :lay_y, :lay_w, :lay_h,
                  :text_wrap

    # Low-level setters and getters
    # =============================

    def anchor=(coords)
        @anchor_x, @anchor_y = coords
        self.align_element
    end

    def dims=(dims); self.w, self.h = dims; end

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
        wrapped_text = self.get_wrapped_text(value)
        super(wrapped_text)
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

    def get_wrapped_text(value)
        # Insert newlines into the text if text wrapping is required
        if self.text_wrap && self.w > 0 && value.length > 0
            $logger.info "Wrapping text"

            text_chunks = value.split(/\n/)

            text_chunks.each_with_index do |chunk,index|
                $logger.info "Parsing text chunk #{chunk}"
                running_index = 0
                while (split_index = self.split_text_at(chunk[running_index..-1], self.w)) > 0
                    $logger.info "Chunk with running index #{running_index} should be split at #{split_index} due to width #{self.w}"
                    running_index += split_index + 1
                    text_chunks[index].insert(running_index, "\n")
                end
                $logger.info "Resulting chunk #{chunk}"
            end

            return text_chunks.join("\n")
        end

        return value
    end

    def align_text
        return if self.text.length == 0

        # Align the text
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

        # Size the element in the pixel-domain, handling integer rounding gracefully
        unless self.lay_w.nil?
            float_width  = self.lay_w * x_lay_size
            self.w = float_width.to_i
            self.w += 1 if float_width - self.w >= 0
        end

        unless self.lay_h.nil?
            float_height = self.lay_h * y_lay_size
            self.h = float_height.to_i
            self.h += 1 if float_height - self.h >= 0
        end

        self.w ||= 0
        self.h ||= 0

        # Align the element properly around the anchor point
        align_element
        align_text unless self.text.nil? || self.text.empty?

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

    def report_positioning
        $logger.info "#{self.name} ldims: #{ldims.inspect}, lay_w: (#{lay_w}), lay_h: (#{lay_h}), anchor_x: (#{anchor_x}), anchor_y: (#{anchor_y})"
    end
end

class Pane < UIElement; end

# This is a superclass that is not meant to be instantiated.
# It relies on the child classes to define behavior for the "resize group" method.
class Grouping < Pane
    def elements; @elements ||= []; end
    def elements=(elem_array)
        @elements = elem_array
        self.elements.each do |element|
            element.parent = self
        end
        resize_group
    end
    def add_element(value)
        value.parent = self
        self.elements << value
        resize_group
    end

    def grouping; @grouping || :column; end
    def grouping=(type)
        @grouping = type
        resize_group
    end

    def resize_group; self.compute_dimensions; end
end

# An ElementGroup is a series of fixed-size elements that are contained within a fixed-size container
class ElementGroup < Grouping
    def element_size
        if @element_size.nil?
            @element_size = (self.grouping == :square_grid) ? [1,1] : 1
        end
        @element_size
    end
	def element_size=(value)
        @element_size = value
	end

    # Which element to begin at when rendering elements.  This is the mechanic that lets us scroll
    #  through the list with a scrollbar when there are more elements that the container can hold
    def element_index; @element_index ||= 0; end
    def scroll_up
        @element_index = [self.element_index-self.elements_per_page, 0].max
    end
    def scroll_down
        @element_index = [self.element_index+self.elements_per_page,
                          self.elements.size-self.elements_per_page-1].min
    end

    def elements_per_page
        case self.grouping
        when :square_grid
            (@manager.looknfeel.full ** 2) / (self.element_size[0] * self.element_size[1])
        else
            @manager.looknfeel.full / self.element_size
        end
    end
    def elements_on_this_page
        self.elements[self.element_index..-1]
    end

    def resize_group
        case self.grouping
        when :square_grid
			elems_x = (@manager.looknfeel.full / self.element_size[0])
			elems_y = (@manager.looknfeel.full / self.element_size[1])

            if (elems_x * elems_y) < self.elements.size
                # A scrollbar is required
                fixme
            end

            elements_on_this_page.each_with_index do |element, index|
                element.snap  = [:bottom, :left]
                element.ldims = [((index%elems_x) * self.element_size[0]),
                                 ((index/elems_x).to_i * self.element_size[1]),
                                 self.element_size[0], self.element_size[1]]
                element.report_positioning
            end
		when :row
            elems = (@manager.looknfeel.full / self.element_size)

            if elems < self.elements.size
                # A scrollbar is required
                fixme
            end

            elements_on_this_page.each_with_index do |element, index|
                element.snap  = [:bottom, :left]
                element.ldims = [(index * self.element_size), 0, self.element_size, @manager.looknfeel.full]
            end
		else # :column
            elems = (@manager.looknfeel.full / self.element_size)

            if elems < self.elements.size
                # A scrollbar is required
                fixme
            end

            elements_on_this_page.each_with_index do |element, index|
                element.snap  = [:bottom, :left]
                element.ldims = [0, (index * self.element_size), @manager.looknfeel.full, self.element_size]
            end
        end
        super
    end

    def fixme
        $logger.info "I'm lazy, and haven't written the code to make ElementGroups create scrollbars for themselves if the number of elements exceeds the group pane's capacity."
    end
end

# An ElementContainer is an area of fixed-size that contains a series of elements
class ElementContainer < Grouping
    def resize_group
        case self.grouping
        when :square_grid
            root = self.elements.size ** 0.5
            rows = ((root == root.to_i) ? root : root + 1).to_i
            elem_width  = @manager.looknfeel.full / rows
            elem_height = @manager.looknfeel.full / rows

            self.elements.each_with_index do |element, index|
                row = index % rows
                col = index / rows
                element.snap  = [:left, :bottom]
                element.ldims = [col*elem_width, row*elem_height, elem_width, elem_height]
            end
        when :row
            elem_width  = @manager.looknfeel.full / self.elements.size.to_f
            elem_height = @manager.looknfeel.full

            self.elements.each_with_index do |element, index|
                # TODO - Make the element order modifiable
                element.snap  = [:left, :bottom]
                element.ldims = [index*elem_width, 0, elem_width, elem_height]
            end
        else # when :column
            elem_width  = @manager.looknfeel.full
            elem_height = @manager.looknfeel.full / self.elements.size.to_f

            self.elements.each_with_index do |element, index|
                element.snap  = [:left, :bottom]
                element.ldims = [0, index*elem_height, elem_width, elem_height]
            end
        end
        super
    end
end

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
        self.text = (self.text + [event.upper!.key].pack("C"))
        # Seems to follow a printf format, where "%%" resolves to '%'
        if event.key == Keyboard.KEY_PERCENT
            self.text = (self.text + [event.upper!.key].pack("C"))
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

    # A TickSlider is accomplished by overloading these two functions of the slider
    #  to use discrete values from a list rather than a range
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

        @manager = args[1]
        half = @manager.looknfeel.lay_divisions / 2

        @snap      = [:center,:center]
        self.ldims = [half,half,half/2,half/2]

        @textbox = @manager.create(Text, {:parent=>self, :text_align=>[:center,:center], :ldims=>[half,-5]})
        ok_button = @manager.create(Button, {:parent=>self, :ldims=>[half,4,10,3], :snap=>[:center,:center], :text=>"OK"}) {
            @manager.kill_element(self)
            @manager.focus_override = nil
        }

        # This prevents users from ignoring the dialog
        @manager.focus_override = [ok_button]
    end

    def text=(value); @textbox.text = value; end
end

class InputDialog < Box
    def initialize(*args, &block)
        super(*args)

        @callback = block_given? ? block : Proc.new { "The InputDialog, it does nothing!" }
        @manager  = args[1]

        half = @manager.looknfeel.lay_divisions / 2

        @snap      = [:center,:center]
        self.ldims = [half,half,half/2,half/2]

        @textbox      = @manager.create(Text, {:parent=>self, :ldims=>[half,-5], :text_align=>[:center,:center]})
        @field        = @manager.create(InputField, {:parent=>self, :ldims=>[half,half,half,3],
                                                    :text_align=>[:left,:center], :snap=>[:center,:center]})
        ok_button     = @manager.create(Button, {:parent=>self, :ldims=>[half/2,4,10,3], :snap=>[:center,:center], :text=>"OK"}) {
            self.submit
        }
        cancel_button = @manager.create(Button, {:parent=>self, :ldims=>[-1-(half/2),4,10,3], :snap=>[:center,:center], :text=>"Cancel"}) {
            self.teardown
        }

        @manager.active_element = self
        @manager.focus_override = [ok_button, cancel_button]
    end

    def text=(value); @textbox.text = value; end

    def teardown
        @manager.focus_override = nil
        @manager.active_element = nil
        @manager.kill_element(self)
    end

    def submit
        ret_value = @callback.call(@field.text)
        self.teardown if ret_value == :accept
    end

    def input_event(event)
        $logger.info "Input dialog receives event #{event.inspect}"
        if event === KeyPressed
            case event.key
            when Keyboard.KEY_a..Keyboard.KEY_z
                @field.push_char(event)
            when Keyboard.KEY_BACKSPACE
                @field.pop_char
            when Keyboard.KEY_RETURN
                self.submit
            end
            :handled
        end
        :unhandled
    end
end

class ListSelection < Box
    def initialize(*manager, &block)
        super(*args)

        @manager = args[1]
        @tracker = block_given? ? block : Proc.new { $logger.info "The ListSelection, it does nothing!" }

        @text_align = [:left, :top]

        # Create a scrollbar
        max_dim = @manager.looknfeel.lay_divisions
        scrollbar_width = 3
        status_height = 3
        @manager.create(Button, {:parent=>self, :text=>"^",
                                 :ldims=>[-1,-1-status_height,scrollbar_width,scrollbar_width],
                                 :snap=>[:right,:top]}) {
            self.list_position = [self.list_position-1, 0].max
        }
        @manager.create(Button, {:parent=>self, :text=>"V",
                                 :ldims=>[-1, 0,scrollbar_width,scrollbar_width],
                                 :snap=>[:right,:bottom]}) {
            self.list_position = [self.list_position+1, [list.size-1,0].max].min
        }

        # Create the list pane
        @list_pane = @manager.create(Pane, {:parent=>self,
                                            :ldims=>[0,-1-status_height,max_dim-scrollbar_width,max_dim-status_height],
                                            :snap=>[:left,:top]})
    end

    def list; @list || []; end
    def list=(array); @list=array; self.regenerate; end

    def list_position; @list_position || 0; end
    def list_position=(value); @list_position=value; self.regenerate; end

    def list_size; @list_size || 8; end
    def list_size=(value); @list_size=value; self.regenerate; end

    def compute_dimensions
        super
        self.regenerate
    end

    def select(index)
        # Return the index to the tracker
        @tracker.call(index)
    end

    def regenerate
        @list_pane.cull_children

        elements = self.list[self.list_position...(self.list_position+self.list_size)]

        max_dim  = @manager.looknfeel.lay_divisions
        dim_size = max_dim / self.list_size

        elements.each_with_index do |item,index|
            this_y = -1 - (dim_size * (index))
            @manager.create(Clickable, {:parent=>@list_pane, :ldims=>[0,this_y,max_dim,dim_size],
                                        :snap=>[:left,:top], :text_align=>[:left,:center],:text=>item}) {
                self.select(index)
            }
        end
    end
end

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

        @manager     = args[1]
        @toggled       = false
        @buffer_length = 26
        @p_eval        = block_given? ? block : Proc.new { $logger.info "The console, it does nothing!" }

        full = @manager.looknfeel.lay_divisions
        half = full / 2

        self.ldims = [0, -2, full, half]
        self.snap  = [:left,:bottom]

        @input_field = @uimanager.create(InputField, {:parent=>self,:ldims=>[0,2,full,2],:text_align=>[:left,:center],:snap=>[:left,:top],:text_wrap=>true})
        self.clear_history
        @history_panel = @uimanager.create(Pane, {:parent=>self,:ldims=>[0,2,full,full-2],:text_align=>[:left,:bottom],:snap=>[:left,:bottom],:text_wrap=>true})
        self.update_history
    end

    def update_history
        @history_panel.text = @display_history[-@buffer_length..-1].join("\n")
    end

    def clear_history
        # stores cmds and cmd results
        @display_history = Array.new(@buffer_length, ">")
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
            when Keyboard.KEY_RETURN
                clear_history if @input_field.text == "clear"
                return :handled if @input_field.text.length == 0

                # Call the proc
                result = call(@input_field.text).to_s.split("\n")
                # add command and results to display history
                @display_history += [@input_field.text] + result
                # add command to command history
                @cmd_history.insert(0, @input_field.text)
                @cmd_placement = nil
                @input_field.text = ""
            when Keyboard.KEY_BACKSPACE
                @input_field.pop_char
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
            else
                if event.printable?
                    @input_field.push_char(event)
                else
                    return :unhandled
                end
            end
        end

        self.update_history
        return :handled
    end

    def toggled?
        @toggled
    end

    def toggle
        move_distance = @manager.looknfeel.lay_divisions / 2 - 1
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
