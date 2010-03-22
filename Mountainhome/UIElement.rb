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

    def elements_at(x, y, d)
        collisions = []
        @children.each do |child|
            child.elements_at(x, y, d+1).each { |subcoll| collisions << subcoll }
        end
        #$logger.info "Searching for collisions at depth #{d}"
        if @clickable and 
           (x >= self.x) and (x <= (self.x + self.w)) and
           (y >= self.y) and (y <= (self.y + self.h))
            #$logger.info "Appending self"
            return collisions << {:element => self, :d => d}
        else
            #$logger.info "#{[x,y].inspect} did not meet criterion #{[self.x,self.y,self.w,self.h,@clickable].inspect}"
            return collisions
        end
    end
end
