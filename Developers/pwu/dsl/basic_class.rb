# 
# class NAME < HASH{:is_a}
#     :attr_accessor name
# end
# 
# def describe(name, hash, &block)
#     # define class:
#     if block_given?
#         yield
# end
# 
# describe :stone, {:is_a => :terrain_type} do
#     looks_like 'stone_terrain.png'
#     on_destruction do |destroyer|
#         drops :stone => rand(3) * destroyer.mining_skill
#     end
# end
require "Class_Creator.bundle"

def describe(name, type)
    new_class = Class_Creator.create_class(name, type)
    # the given block contains setup functions for the new class
    if block_given?
        yield(new_class)
    end
end

class Entity
    attr_accessor :name
end

describe "Terrain_Type", Entity

describe "Stone", Terrain_Type do |stone|
    # @polygons = 87
    # puts @polygons
    # looks_like 'stone_terrain.png'
    # on_destruction do |destroyer|
    #     drops :stone => rand(3) * destroyer.mining_skill
    # end
end

describe "Animal", Entity
describe "Dwarf", Animal

embarkers = Array.new(7) { Dwarf.new }
embarkers[0].name = "Freeflorp Frighaven"
embarkers[1].name = "Druzniin Drapgoor"
puts embarkers[0].class