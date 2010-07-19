class Path < MHPath
    def initialize(world, source, dest)
        super(world, source[0], source[1], source[2], dest[0], dest[1], dest[2])
    end
end
