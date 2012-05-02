class MountainhomeDSL
    # Items
    describe :item, :base => "Item", :managed_by => ItemManager
    describe :boulder, :instantiable => true, :is_a => [:item]
    describe :food, :instantiable => true, :is_a => [:item]

    #########
    # TILES #
    #########

    describe :voxel,
        :extends => [:voxel_parameters],
        :base => "Voxel" do
        set_class_attributes(
            :shader        => "terrain.shader",
            :textureSet    => "clean",
            :diffuseFactor => 1,
            :ambientFactor => 0.7)

        set_parameters :selected => false
    end

    describe :multitexture_voxel,
        :is_a => [:voxel] do
        set_class_attributes :shader => "multitexture_terrain.shader"
    end

    describe :wall,
        :instantiable => true,
        :is_a => [:voxel] do
        set_class_attributes(
            # FIXME: These shouldn't be hardcoded, but should vary based on
            # what was used to create the wall.
            :texture => "wall.png",
            :drops => :boulder )
    end

    describe :rock,
        :instantiable => true,
        :is_a => [:voxel] do
        set_class_attributes(
            :texture => "rock.png",
            :drops => :boulder)
    end

    describe :dirt,
        :instantiable => true,
        :is_a => [:voxel] do
        set_class_attributes(
            :texture => "dirt.png",
            :grows => :grass)
    end

    describe :grass,
        :instantiable => true,
        :is_a => [:multitexture_voxel] do
        set_class_attributes(
            :bottomTexture => "dirt.png",
            :sideTexture   => "dirt_grass.png",
            :topTexture    => "grass.png")
    end

    describe :liquid,
        :is_a => [:voxel] do
        set_class_attributes :flow_rate => 500 # How many ms elapse before a tick
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

    describe :creature, :is_a => [:natural], :extends => [:movement], :managed_by => AIManager do
        set_class_attributes :ai_routine => ForagerAI.new
        set_class_attributes :move_speed => 0.02
    end

    describe :dwarf, :is_a => [:creature], :instantiable => true, :extends => [:worker, :inventory] do
        set_class_attributes :ai_routine => DwarfAI.new
        set_instance_attributes :nutrition => 100
        set_instance_attributes :hp => 1
        has_slots Body, Right_Hand, Left_Hand, Mouth
    end
end
