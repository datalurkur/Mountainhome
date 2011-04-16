class Array
    def x; self[0]; end
    def x=(val); self[0]=val; end
    def y; self[1]; end
    def y=(val); self[1]=val; end
    def z; self[2]; end
    def z=(val); self[2]=val; end
    def normalize
        magnitude = (self.inject(0) { |sum,i| sum + (i**2) }) ** 0.5
        self.collect { |i| i / magnitude }
    end
end

class Integer
    def power_of_two?
        Math.log2(self).denominator == 1
    end
end

class HeightMap
    attr_reader :size, :max_x, :max_y, :extrema
    def initialize(args={})
        @size  ||= args[:size]
        @max_x   = @size.x - 1
        @max_y   = @size.y - 1
        @extrema = [nil,nil]

        @map = Array.new(@size.x) { Array.new(@size.y, nil) }

        self
    end

    def data; @map; end

    def recompute_extrema
        @extrema = [
            @map.collect(&:min).min,
            @map.collect(&:max).max
        ]
    end

    def clamp_to!(range)
        pre_trans  = -@extrema.min
        pre_scale  = (@extrema.max - @extrema.min)
        post_scale = (range.max - range.min).to_f
        post_trans = range.min
        @map.each do |row|
            row.collect! do |val|
                normalized_val = (val + pre_trans) / pre_scale
                ((normalized_val * post_scale) + post_trans).to_i
            end
        end
        recompute_extrema
    end

    def displace_by(heightmap, max)
        @map.each_with_index do |row,x|
            row.each_with_index do |val,y|
                @map[x][y] = [val + heightmap.data[x][y], max].min
            end
        end
        recompute_extrema
    end

    def mix_with(heightmap, amount)
        @map.each_with_index do |row,x|
            row.each_with_index do |val,y|
                val = (val * (1.0 - amount)) + (heightmap.data[x][y] * amount)
            end
        end
        recompute_extrema
    end

    def build; @map.each { |row| row.collect! { |col| col.to_i } }; end
    def self.build(args={}); hmap = self.new(args); hmap.build; hmap; end

    def seed_edge(heightmap, edge=:left)
        edge_size = case edge
            when :bottom, :top; [heightmap.size.x, @size.x].min
            else; [heightmap.size.y, @size.y].min
        end

        (0...edge_size).each do |i|
            case edge
            when :right
                @map[@max_x][i] = heightmap.data[0][i]
            when :top
                @map[i][@max_y] = heightmap.data[i][0]
            when :bottom
                @map[i][0] = heightmap.data[i][heightmap.max_y]
            else
                @map[0][i] = heightmap.data[heightmap.max_x][i]
            end
        end
    end

    def stitch(heightmap, edge)
        case edge
        when :left, :right
            if (heightmap.size.y != @size.y)
                $logger.error "Cannot stitch heightmaps of different edge sizes together"
                return
            end
            @map = (edge == :left) ?
                heightmap.data + @map[1..-1] :
                @map + heightmap.data[1..-1]
            @size.x = @size.x + heightmap.size.x - 1
        when :top, :bottom
            if (heightmap.size.x != @size.x)
                $logger.error "Cannot stitch heightmaps of different edge sizes together"
                return
            end
            new_data = []
            @map.each_with_index do |row,i|
                new_data << ((edge == :bottom) ?
                    heightmap.data[i] + row[1..-1] :
                    row + heightmap.data[i][1..-1])
            end
            @map = new_data
            @size.y = @size.y + heightmap.size.y - 1
        end

        @max_x = @size.x - 1
        @max_y = @size.y - 1
        recompute_extrema
    end
end

class MidPoint < HeightMap
    def initialize(args={})
        @scaling = args[:scaling] || 0.5
        @size    = [2**args[:power] + 1]*2 if args[:power]

        super(args)

        @size = [1,1] unless self.valid_size?
    end

    def valid_size?
        @size.each do |size|
            unless (size - 1).power_of_two?
                $logger.error "#{size} is not a valid dimension for the Midpoint Displacement algorithm"
                return false
            end
        end
    end

    def extend(direction=:right)
        extension = MidPoint.new(:size => @size, :scaling => @scaling)
        case direction
        when :left; extension.seed_edge(self, :right)
        when :right; extension.seed_edge(self, :left)
        when :bottom; extension.seed_edge(self, :top)
        when :top; extension.seed_edge(self, :bottom)
        end
        extension.build
        self.stitch(extension, direction)
    end

    def build(args={})
        return unless self.valid_size?

        # Seed the initial four corners (unless they already exist,
        #  such as in the event we're extending a previously existing heightmap)
        @map[0][0]           ||= rand()
        @map[0][@max_y]      ||= rand()
        @map[@max_x][0]      ||= rand()
        @map[@max_x][@max_y] ||= rand()

        # Starting with a single large square, compute the diamond and square steps for an
        #  increasingly more granular graph, decreasing the scale of random heightmap
        #  perturbation as the grain becomes finer
        size  = @max_x
        scale = 1.0
        while size > 1
            steps = @max_x / size
            half_size = size / 2

            steps.times do |x|
                steps.times do |y|
                    # Compute the average of four corners, determine a midpoint, and form 4 diamonds
                    x0 = (size * x)
                    y0 = (size * y)
                    x1 = x0 + size
                    y1 = y0 + size

                    # Average the four corners of the square
                    avg = (@map[x0][y0] + @map[x0][y1] + @map[x1][y0] + @map[x1][y1]) / 4.0

                    # Compute the midpoint of the diagonals between these points by adding a random amount to the average
                    @map[x0+half_size][y0+half_size] = avg + (2*scale*rand()-scale)
                end
            end

            steps.times do |x|
                steps.times do |y|
                    # Compute the midpoints for each of the diamonds formed
                    # Note: We only compute the upper and right diamonds if they're the last ones in the row/column
                    #  (otherwise, the iterative nature of the algorithm causes many values to be computed twice)
                    x0    = (size * x)
                    y0    = (size * y)
                    x1    = x0 + size
                    y1    = y0 + size
                    x_mid = x0 + half_size
                    y_mid = y0 + half_size
                    x_sub = x0 - half_size
                    y_sub = y0 - half_size

                    # Packing the diamond code into a method makes this look nicer, but is lacking in performance
                    # Left diamond
                    if @map[x0][y_mid].nil?
                        sum = @map[x_mid][y_mid] + @map[x0][y0] + @map[x0][y1]
                        avg = if x_sub >= 0
                            (sum + @map[x_sub][y_mid]) / 4.0
                        else
                            sum / 3.0
                        end
                        @map[x0][y_mid] = avg + (2*scale*rand()-scale)
                    end

                    # Bottom diamond
                    if @map[x_mid][y0].nil?
                        sum = @map[x_mid][y_mid] + @map[x0][y0] + @map[x1][y0]
                        avg = if y_sub >= 0
                            (sum + @map[x_mid][y_sub]) / 4.0
                        else
                            sum / 3.0
                        end
                        @map[x_mid][y0] = avg + (2*scale*rand()-scale)
                    end

                    # Right diamond
                    # Only compute the right diamond if we're at the further right boundary
                    if (x==(steps-1)) && @map[x1][y_mid].nil?
                        x_sup = x1 + half_size
                        sum = @map[x_mid][y_mid] + @map[x1][y0] + @map[x1][y1]
                        avg = sum / 3.0
                        @map[x1][y_mid] = avg + (2*scale*rand()-scale)
                    end

                    # Top diamond
                    # Only compute the top diamond if we're at the further top boundary
                    if (y==(steps-1)) && @map[x_mid][y1].nil?
                        y_sup = y1 + half_size
                        sum = @map[x_mid][y_mid] + @map[x0][y1] + @map[x1][y1]
                        avg = sum / 3.0
                        @map[x_mid][y1] = avg + (2*scale*rand()-scale)
                    end
                end
            end

            size  /= 2
            scale *= @scaling
        end

        recompute_extrema
    end
end

class Voronois < HeightMap
    def initialize(args={})
        @features       = args[:features] || []
        random_features = args[:random_features] || 0
        @coefficients   = args[:coefficients] || [-1,1]

        super(args)

        randoms = 0
        until randoms >= random_features && @features.size >= @coefficients.size
            @features << [rand(@size.x),rand(@size.y)]
            @features.uniq!
            randoms += 1
        end
    end

    def build
        # Iterate across the heightmap, setting the magnitude of each point
        #  according to its distance from the features
        @size.x.times do |x|
            @size.y.times do |y|
                # Compute the distance to each feature
                xP = x + Math.sin(y)
                yP = y - Math.sin(x)
                feat_dist = @features.collect do |feature|
                    ((feature.x - xP)**2 + (feature.y - yP)**2)**0.5
                end
                # Sort the features by distance
                feat_dist.sort!

                # Compute the value at this location using the closest n features
                h_value = 0
                @coefficients.each_with_index do |coeff,i|
                    h_value += (feat_dist[i] * coeff)
                end
                @map[x][y] = h_value
            end
        end
        recompute_extrema
    end
end

class SimplexNoise < HeightMap
    class << self
        def gradients
            [
                [ 1,  1], [-1,  1], [-1, -1],
                [ 1, -1], [ 0,  1], [ 0, -1],
                [ 0, -1], [ 1,  0], [-1,  0]
            ]
        end
        def f_factor; ((3.0 ** 0.5) - 1.0) / 2.0; end
        def g_factor; (3.0 - (3.0 ** 0.5)) / 6.0; end
        def p_table
            @p_table ||= Array.new(256) { |i| i }.sort_by { rand } * 2
            @p_table
        end
    end

    def initialize(args={})
        @falloff = args[:falloff] || 2
        @octaves = args[:octaves] || 8

        super(args)
    end

    def noise_at(x, y)
        # Skew the input space to determine which cell we're in
        f = SimplexNoise.f_factor * (x + y)
        i = (x + f).to_i
        j = (y + f).to_i

        g = SimplexNoise.g_factor * (i + j)
        cell_x = i - g
        cell_y = j - g

        x0 = x - cell_x
        y0 = y - cell_y

        # Which triangle is this?
        i1, j1 = (x0 > y0) ? [1,0] : [0,1]

        # Compute the locations of the other two corners
        x1 = x0 - i1 + SimplexNoise.g_factor
        y1 = y0 - j1 + SimplexNoise.g_factor
        x2 = x0 - 1.0 + (2.0 * SimplexNoise.g_factor)
        y2 = y0 - 1.0 + (2.0 * SimplexNoise.g_factor)

        # Determine the gradient indices of each corner
        ii = i & 0xff
        jj = j & 0xff

        # Get the gradients for each corner using the permutation table
        gi0 = SimplexNoise.p_table[ii + SimplexNoise.p_table[jj]] % 9
        gi1 = SimplexNoise.p_table[ii + i1 + SimplexNoise.p_table[jj + j1]] % 9
        gi2 = SimplexNoise.p_table[ii + 1 + SimplexNoise.p_table[jj + 1]] % 9

        # Determine the contributions of each corner
        t0 = 0.5 - (x0**2) - (y0**2)
        n0 = if t0 < 0
            0
        else
            t0 *= t0
            grad0 = SimplexNoise.gradients[gi0]
            (t0**2) * ((grad0[0]*x0) + (grad0[1]*y0))
        end
        t1 = 0.5 - (x1**2) - (y1**2)
        n1 = if t1 < 0
            0
        else
            t1 *= t1
            grad1 = SimplexNoise.gradients[gi1]
            (t1**2) * ((grad1[0]*x1) + (grad1[1]*y1))
        end
        t2 = 0.5 - (x2**2) - (y2**2)
        n2 = if t2 < 0
            0
        else
            t2 *= t2
            grad2 = SimplexNoise.gradients[gi2]
            (t2**2) * ((grad2[0]*x2) + (grad2[1]*y2))
        end

        70 * (n0 + n1 + n2)
    end

    def build
        coefficients = Array.new(@octaves) { |i| 1.0 / (@falloff ** i) }

        @size.x.times do |x|
            @size.y.times do |y|
                local_value = 0
                coefficients.each do |c|
                    l_x = x.to_f / (@size.x * c)
                    l_y = y.to_f / (@size.y * c)
                    local_value += (c * noise_at(l_x, l_y))
                end
                @map[x][y] = local_value
            end
        end

        recompute_extrema
    end
end
