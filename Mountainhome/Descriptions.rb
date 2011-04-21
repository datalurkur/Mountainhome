class MountainhomeDSL
    #########
    # TILES #
    #########

    describe :tile, :base => "Tile", :extends => [:tile_parameters] do
        set_class_attributes(
            :shader        => "terrain.shader",
            :textureSet    => "clean",
            :diffuseFactor => 1,
            :ambientFactor => 0.7)

        set_parameters(:selected => false)
    end

    describe :multitexture_tile, :is_a => [:tile] do
        set_class_attributes :shader => "multitexture_terrain.shader"
    end

    describe :rock, :is_a => [:tile], :instantiable => true do
        set_class_attributes :texture => "rock.png"
    end

    describe :dirt, :is_a => [:multitexture_tile], :instantiable => true do
        set_class_attributes(
            :bottomTexture => "dirt.png",
            :sideTexture   => "dirt_grass.png",
            :topTexture    => "grass.png")
    end

    describe :liquid, :is_a => [:tile] do
        set_parameters(:liquid_level => 10)
    end

    describe :water, :is_a => [:liquid], :instantiable => true do
        set_class_attributes :texture => "water1.png"
    end

    ##########
    # ACTORS #
    ##########

    describe :natural, :base => "Actor"

    describe :plant, :is_a => [:natural], :managed_by => PlantManager do
        set_class_attributes :minimum_population => 1
    end

    describe :tree, :is_a => [:plant], :instantiable => true do
        set_class_attributes(
            :model => "Tree",
            :material => "green-lit"
        )
    end

    describe :creature, :is_a => [:natural], :uses => [:movement]

    describe :dwarf, :is_a => [:creature], :instantiable => true, :uses => [:worker] do
        set_instance_attributes :hp => 1
    end

end
