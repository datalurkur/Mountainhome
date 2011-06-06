@max_size     = 100.0
@liquid_grid = [
    [0.0, 0.0, 0.0, @max_size / 2.0],
    [0.0, 0.0, 0.0, 0.0],
    [0.0, 0.0, 0.0, @max_size / 5.0],
    [0.0, 0.0, 0.0, 0.0]
]
@liquid_speed = 1.5
@threshold    = @max_size / 10000.0

@dirty_nodes = [[0,3],[2,3]]

def get(coords)
    @liquid_grid[coords[0]][coords[1]]
end

def set(coords, level)
    @liquid_grid[coords[0]][coords[1]] = level
end

def step
    affected_nodes = @dirty_nodes.dup
    @dirty_nodes.each do |node|
        this_height = get(node)

        lower_neighbors = []
        (-1..1).collect do |x|
            (-1..1).collect do |y|
                next if x == 0 && y == 0
                local_coords = [node[0] + x, node[1] + y]
                next if oob?(local_coords)
                local_height = get(local_coords)
                (lower_neighbors << local_coords) if ((this_height - local_height) > @threshold)
            end
        end

        affected_nodes.concat(lower_neighbors)

        # We shoot for the average of this node with its surrounds
        target_height = ((lower_neighbors.inject(0) { |sum, local| sum + get(local) } + this_height) / (lower_neighbors.size + 1))

        lower_neighbors.each do |neighbor|
            delta = target_height - get(neighbor)
            if delta > @liquid_speed
                delta = @liquid_speed
            end

            set(node, get(node) - delta)
            set(neighbor, get(neighbor) + delta)
        end
    end

    print_grid

    @dirty_nodes = []
    affected_nodes.uniq.each do |node|
        this_level = get(node)
        (-1..1).each do |x|
            (-1..1).each do |y|
                next if x==0 && y==0
                local = [node[0]+x,node[1]+y]
                next if oob?(local)
                local_level = get(local)
                if (this_level - local_level) > @threshold
                    @dirty_nodes << node
                elsif (local_level - this_level) > @threshold
                    @dirty_nodes << local
                end
            end
        end
    end
    @dirty_nodes.uniq!
end

def oob?(coords)
    (coords[0] >= @liquid_grid.size || coords[1] >= @liquid_grid.first.size || coords[0] < 0 || coords[1] < 0)
end

def print_state
    puts "Grid:"
    @liquid_grid.each do |x|
        puts "\t#{x.inspect}"
    end
    puts "Dirty nodes: #{@dirty_nodes.inspect}"
end

def print_grid
    puts "INT GRID:"
    @liquid_grid.each do |x|
        puts "\t#{x.collect { |y| y.to_i }}"
    end
end

puts "STARTING GRID:"
print_grid

1000.times do |x|
    break if @dirty_nodes.empty?
    puts "\n" + "=" * 50
    puts "Iteration #{x}"
    step
end

print_grid
