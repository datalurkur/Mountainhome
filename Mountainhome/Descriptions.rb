require 'PlantManager'

# Eventually, this will be a place where we require various sub-description files,
#  such as Tiles.rb, Plants.rb, etc
# Since there are a low volume presently, they can just all reside here

describe :natural, :base => "Actor" do
  has_attributes :position
  attribute_values(:position => [0,0,0])
=begin
  attribute_values(
    :hp     => Proc.new { (level * hd / 2.0).floor + ((con - 10) / 2) },
    :weight => Proc.new { con * hd },
    :position => [0,0,0],
    :level  => 1)
=end
end

describe :tile, :base => "Tile" do
  has_attributes :rarity, :grouping_type, :material
end

describe :liquid do
end

describe :plant, :is_a => [:natural, :instantiable], :managed_by => PlantManager do
    has_attributes :minimum_population
end

describe :dwarf, :is_a => [:natural, :instantiable] do
=begin
  attribute_values(
    :con => 16,
    :hd  => 12)
=end
end

describe :bedrock, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'bedrock')
end

describe :hardrock, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'hardrock')
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

describe :empty, :is_a => [:tile, :instantiable] do
  attribute_values(
    :rarity        => :common,
    :grouping_type => :large_expanses,
    :material      => 'empty')
end
