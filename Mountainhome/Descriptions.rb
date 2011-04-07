class MountainhomeDSL
    # Eventually, this will be a place where we require various sub-description files,
    #  such as Tiles.rb, Plants.rb, etc
    # Since there are a low volume presently, they can just all reside here
    describe :tile, :base => "Tile", :extends => [:tile_parameters] do
        has_attributes :rarity, :grouping_type
        has_class_attrs(
            :shader  => "terrain.shader",
            :texture => "grass_4096_tiled.jpg"
        )
    end

    describe :natural, :base => "Actor"

    describe :plant, :is_a => [:natural], :managed_by => PlantManager do
        has_class_attrs(
            :minimum_population => 1
        )
    end

    describe :fern, :is_a => [:plant, :instantiable]

    describe :creature, :is_a => [:natural], :uses => [:movement]

    describe :dwarf, :is_a => [:creature, :instantiable], :uses => [:worker] do
        has_attributes :hp
        default_value(:hp => 1)
    end

    describe :terra, :is_a => [:tile] do
        has_parameter(:selected, false)
    end

    describe :gravel, :is_a => [:terra, :instantiable] do
        has_class_attr("texture", "rock_2048_tiled.jpg")
        default_values(
            :rarity        => :common,
            :grouping_type => :large_expanses
        )
    end

    describe :grass, :is_a => [:terra, :instantiable] do
        has_class_attr(:texture, "grass_4096_tiled.jpg")
        default_values(
            :rarity        => :common,
            :grouping_type => :large_expanses
        )
    end

    describe :softrock, :is_a => [:terra, :instantiable] do
        has_class_attr(:texture, "rock_2_4w.jpg")
        default_values(
            :rarity        => :common,
            :grouping_type => :large_expanses
        )
    end

    describe :sediment, :is_a => [:terra, :instantiable] do
        has_class_attr(:texture, "rock_2_4w.jpg")
        default_values(
            :rarity        => :common,
            :grouping_type => :large_expanses
        )
    end

    describe :liquid, :is_a => [:tile] do
        has_parameter(:liquid_level, 10)
    end

    describe :water,  :is_a => [:liquid, :instantiable] do
        has_class_attr(:texture, "rock_2_4w.jpg")
        default_values(
            :rarity        => :common,
            :grouping_type => :large_expanses
        )
    end
end
