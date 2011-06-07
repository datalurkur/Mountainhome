@grid = [
    [0, 0, 0, 8],
    [0, 0, 0, 0],
    [0, 7, 2, 5],
    [0, 0, 0, 8]
]

@dirty_nodes = [[0,3],[2,3],[2,1]]

def print
    str = @grid.collect do |row|
        row.collect do |col|
            "#{col}"
        end.join(" ")
    end.join("\n")
    puts str
end
def oob?(coords)
    coords[0] < 0 || coords[1] < 0 || coords[0] >= @grid.size || coords[1] >= @grid.first.size
end
def get(coords)
    @grid[coords[0]][coords[1]]
end
def set(coords,val)
    @grid[coords[0]][coords[1]] = val
end
def step
    affected = @dirty_nodes.dup
    @dirty_nodes.each do |node|
        node_height = get(node)

        # Collect neighbors of this node
        lower_neighbors = []
        (-1..1).each do |x|
            (-1..1).each do |y|
                next if x == 0 && y == 0
                local = [node[0] + x, node[1] + y]
                next if oob?(local)
                local_height = get(local)

                if local_height < node_height
                    lower_neighbors << [local, local_height]
                end
            end
        end
        next if lower_neighbors.empty?

        # Determine how much can spread based on the highest lower neighbor
        highest = lower_neighbors.collect { |x| x[1] }.max

        # Only allow liquid to flow one unit per tick in a given direction
        available = [node_height - highest, lower_neighbors.size].min

        # Starting with the lowest node, work our way upwards until we run out of liquid or neighbors
        used = 0
        lower_neighbors.sort { |x,y| x[1] <=> y[1] }.each do |neighbor|
            break if used == available
            set(neighbor[0], get(neighbor[0]) + 1)
            affected << neighbor[0]
            used += 1
        end
        set(node, get(node) - used)
    end

    @dirty_nodes = []
    affected.uniq.each do |node|
        node_height = get(node)
        (-1..1).each do |x|
            (-1..1).each do |y|
                next if x == 0 && y == 0
                local = [node[0] + x, node[1] + y]
                next if oob?(local)
                local_height = get(local)
                if node_height > local_height
                    @dirty_nodes << node
                elsif node_height < local_height
                    @dirty_nodes << local
                end
            end
        end
    end
    @dirty_nodes.uniq!
end

100.times do |i|
    puts "=" * 100
    puts "Iteration #{i}"
    step
    print
end
