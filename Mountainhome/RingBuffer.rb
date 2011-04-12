# Maintains a contiguous buffer within a larger array. Both sides of the buffer
# can be added to and removed from in constant time, making it useful for things
# like scrollback.
class RingBuffer
    attr_reader :size

    def initialize(max_size)
        @ary = Array.new(max_size)
        @start = 0
        @size = 0
    end

    def [](index); @size > 0 ? @ary[array_index(index)] : nil; end
    def []=(index, value); @ary[array_index(index)] = value; end

    def each(&block)
        (0..(@size - 1)).each do |i|
            yield self[i]
        end
    end

    def reverse_each(&block)
        (1..@size).each do |i|
            yield self[-i]
        end
    end

    def concat(items); items.each { |item| push(item) }; end

    def <<(item); push(item); end
    def push(item);
        if @size == @ary.size
            @ary[@start] = item
            self.start_forward
        else
            # Avoid array_index, as we specifically don't want to mod @size by @size.
            @ary[(@start + @size) % @ary.size] = item
            @size += 1
        end

        item
    end

    def pop()
        if @size > 0
            @size -= 1
            @ary[@start + @size]
        end
    end

    def unshift(item)
        @size += 1 unless @size == @ary.size
        self.start_backward
        @ary[@start] = item
    end

    def shift()
        if @size > 0
            @size -= 1
            self.start_forward
            @ary[@start - 1]
        end
    end

    def inspect
        "[start: #{@start} size: #{@size} buffer: #{@ary.inspect}]"
    end

protected
    def start_forward;  @start = (@start + 1) % @ary.size; end
    def start_backward; @start = (@start - 1) % @ary.size; end
    def array_index(index)
        @size == 0 ? 0 : (@start + (index % @size)) % @ary.size
    end
end
