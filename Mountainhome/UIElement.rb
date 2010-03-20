class UIElement < MHUIElement
    def initialize(name, manager, mat)
        @children = []
        super(name, manager, mat)
    end
    
    def set_dimensions(x,y,w,h)
        super(x,y,w,h)
    end
    
    def add_child(child)
        @children << child
        super(child)
    end
end
