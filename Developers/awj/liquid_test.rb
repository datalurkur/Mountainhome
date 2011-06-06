@liquid_grid = [
    [0.0, 0.0, 0.0, 5.0],
    [0.0, 0.0, 0.0, 0.0],
    [0.0, 0.0, 0.0, 3.0],
    [0.0, 0.0, 0.0, 0.0]
]

@dirty_nodes = [[0,3],[2,3]]
@next_dirty_nodes = []

def get(coords)
    @liquid_grid[coords[0]][coords[1]]
end

def set(coords, level)
    @liquid_grid[coords[0]][coords[1]] = level
end

def step
    @dirty_nodes.each do |node|
        flow_into = []
        to_dirty  = []

        this_level = get(node)
        (-1..1).each do |x_p|
            (-1..1).each do |y_p|
                next if x_p == 0 && y_p == 0
                local = [node[0] + x_p, node[1] + y_p]
                next if oob?(local)
                local_level = get(local)
                if local_level < this_level
                    flow_into << local
                end
                to_dirty << local
            end
        end
        next if flow_into.size == 0

        @next_dirty_nodes.concat(to_dirty).uniq!

        sorted_heights = flow_into.collect { |x| get(x) }.uniq.sort

        sorted_heights.each do |target_height|
            available = this_level - target_height
            puts "Normalizing neighbors to height #{target_height} with #{available} volume available"
            filtered = flow_into.select { |x| get(x) < target_height }
            required = filtered.inject(0) { |sum,x| sum + (target_height - get(x)) }
            if required > available
                each_contribution = available / filtered.size
                filtered.each { |x| set(x, get(x) + each_contribution) }
                set(node, get(node) - available)

                print_state
                break
            else
                filtered.each { |x| set(x, target_height) }
                set(node, get(node) - required)

                print_state
            end
        end

        max = flow_into.collect { |x| get(x) }.max
        if get(node) != max
            avg = (get(node) + (max * flow_into.size)) / (flow_into.size + 1)
            ind_contr = avg - max
            contribution = ind_contr * flow_into.size
            flow_into.each do |x|
                set(x, get(x) + ind_contr)
            end
            set(node, get(node) - contribution)
        end
    end

    @dirty_nodes = @next_dirty_nodes
    @next_dirty_nodes = []
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

10.times do |x|
    puts "Iteration #{x}"
    step
end
