class MountainhomeDSL
    #########
    # TILES #
    #########

    describe :tile, :base => "Tile", :extends => [:tile_parameters] do
        set_class_attributes(
            :shader        => "terrain.shader",
            :textureGrid   => "terrain_cartoon.png",
            :diffuseFactor => 1,
            :ambientFactor => 0.7)

        has_parameter(:selected, false)
    end

    describe :multitexture_tile, :is_a => [:tile] do
        set_class_attributes :shader => "multitexture_terrain.shader"
    end

    describe :rock, :is_a => [:tile, :instantiable] do
        set_class_attributes :gridOffsets => [15, 1]
    end

    describe :dirt, :is_a => [:multitexture_tile, :instantiable] do
        set_class_attributes(
            :bottomGridOffsets => [15, 2],
            :sideGridOffsets   => [15, 3],
            :topGridOffsets    => [15, 0])
    end

    ##########
    # ACTORS #
    ##########

    describe :natural, :base => "Actor"

    describe :plant, :is_a => [:natural], :managed_by => PlantManager do
        set_class_attributes :minimum_population => 1
    end

    describe :tree, :is_a => [:plant, :instantiable] do
        set_class_attributes(
            :model => "Tree",
            :material => "green-lit"
        )
    end

    describe :creature, :is_a => [:natural], :uses => [:movement]

    describe :dwarf, :is_a => [:creature, :instantiable], :uses => [:worker] do
        set_attributes :hp => 1
    end

end
