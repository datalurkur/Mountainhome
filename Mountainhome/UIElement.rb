class UIElement < MHUIElement
    def initialize(name, manager, mat)
		$logger.info "UIElement is initializing"
        super(name, manager, mat)
    end
    
    def set_dimensions(x,y,z,w,h,d)
        super(x,y,z,w,h,d)
    end
end
