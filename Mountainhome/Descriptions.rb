class MountainhomeDSL
    # Items
    describe :item, :base => "Actor", :managed_by => ItemManager
    describe :boulder, :instantiable => true, :is_a => [:item]

    #########
    # TILES #
    #########

    describe :tile,
        :extends => [:tile_parameters],
        :base => "Tile" do
        set_class_attributes(
            :shader        => "terrain.shader",
            :textureSet    => "clean",
            :diffuseFactor => 1,
            :ambientFactor => 0.7)

        set_parameters :selected => false
        set_parameters :to_mine  => false
    end

    describe :multitexture_tile,
        :is_a => [:tile] do
        set_class_attributes :shader => "multitexture_terrain.shader"
    end

    describe :rock,
        :instantiable => true,
        :is_a => [:tile] do
        set_class_attributes(
            :texture => "rock.png",
            :drops => :boulder)
    end

    describe :dirt,
        :instantiable => true,
        :is_a => [:tile] do
        set_class_attributes(
            :texture => "dirt.png",
            :grows => :grass)
    end

    describe :grass,
        :instantiable => true,
        :is_a => [:multitexture_tile] do
        set_class_attributes(
            :bottomTexture => "dirt.png",
            :sideTexture   => "dirt_grass.png",
            :topTexture    => "grass.png")
    end

    describe :liquid,
        :is_a => [:tile] do
        set_parameters :liquid_level => 10
        remove_parameter :selected
    end

    describe :water,
        :instantiable => true,
        :is_a => [:liquid] do
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
