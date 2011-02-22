=begin
NOTES
    -Thinking the element alignment is going to go away, in favor of letting the looknfeel care about things like that
        Not sure how this is going to manifest yet, since I can think of certain cases where it might be useful to specify alignment
        It might be easiest to just pick one convention and stick with it (example: the element position always indicates its lower-left corner)
=end

class UIElement < MHUIElement
    # Hmm.  In the new system, it seems like this is just going to be an empty husk.
    # Keeping it around in case we come up with something to include here later
end

class Label < UIElement
end

class InputField < UIElement
end

class Button < UIElement
end

class Slider < UIElement
end

class Grouping < UIElement
end
