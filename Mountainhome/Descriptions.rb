class MountainhomeDSL
    # Eventually, this will be a place where we require various sub-description files,
    #  such as Tiles.rb, Plants.rb, etc
    # Since there are a low volume presently, they can just all reside here

    describe :tile, :base => "Tile" do
        has_attributes :rarity, :grouping_type, :material
    end

    describe :natural, :base => "Actor"

    describe :plant, :is_a => [:natural], :managed_by => PlantManager do
        has_attributes :minimum_population
    end

    describe :fern, :is_a => [:plant, :instantiable] do
        attribute_values(:minimum_population => 1)
    end

    describe :creature, :is_a => [:natural], :uses => [:movement]

    describe :dwarf, :is_a => [:creature, :instantiable], :uses => [:worker]

    describe :gravel, :is_a => [:tile, :instantiable] do
      attribute_values(
        :rarity        => :common,
        :grouping_type => :large_expanses,
        :material      => 'gravel')
    end

    describe :grass, :is_a => [:tile, :instantiable] do
      attribute_values(
        :rarity        => :common,
        :grouping_type => :large_expanses,
        :material      => 'grass')
    end

    describe :softrock, :is_a => [:tile, :instantiable] do
      attribute_values(
        :rarity        => :common,
        :grouping_type => :large_expanses,
        :material      => 'softrock')
    end

    describe :sediment, :is_a => [:tile, :instantiable] do
      attribute_values(
        :rarity        => :common,
        :grouping_type => :large_expanses,
        :material      => 'sediment')
    end

    describe :liquid, :is_a => [:tile]
    describe :water,  :is_a => [:liquid, :instantiable] do
        attribute_values(
          :rarity        => :common,
          :grouping_type => :large_expanses,
          :material      => 'sediment')
    end
end
