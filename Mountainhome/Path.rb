class Path < MHPath
    attr_accessor :blocked

    def initialize(world, source, dest)
        ret = super(world, source[0], source[1], source[2], dest[0], dest[1], dest[2])

        if !ret
            self.blocked = true
        end
    end
end
