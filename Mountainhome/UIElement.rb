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
