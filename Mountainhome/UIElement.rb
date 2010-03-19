class UIElement < MHUIElement
    def initialize(name, manager, mat)
		$logger.info "UIElement is initializing"
        super(name, manager, mat)
    end
    
    def set_dimensions(x,y,w,h)
        super(x,y,w,h)
    end
end
