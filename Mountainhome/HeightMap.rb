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
                val = [val + heightmap.data[x][y], max].min
            end
        end
    end

    def mix_with(heightmap, amount)
        @map.each_with_index do |row,x|
            row.each_with_index do |val,y|
                val = (val * (1.0 - amount)) + (heightmap.data[x][y] * amount)
            end
        end
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

        @coefficients    = args[:coefficients] || [-1,1]

        feature_deficit = @coefficients.size - (@features.size + random_features)
        random_features += feature_deficit if feature_deficit > 0

        super

        random_features.times do
            @features << [rand(@size.x),rand(@size.y)]
        end
        @features.uniq!
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
