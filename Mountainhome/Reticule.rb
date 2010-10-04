class Reticule < MHReticle
    def initialize(name, manager, x, y, w, h, args={})
        super(name, manager, "blue", "", "", args)
        set_dimensions(x, y, w, h)
    end
end
