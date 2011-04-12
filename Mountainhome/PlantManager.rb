class GenericManager
    def initialize
        @child_types = Array.new
        clear
    end

    def clear
        @child_hash = Hash.new
    end

    def register(klass)
        if @child_types.include? klass
            $logger.info "#{klass} has already been registered with #{self.class}"
        else
            $logger.info "Registering #{klass} with #{self.class}"
            @child_types << klass
        end
    end

    def create_child(world, klass, name, position)
        if @child_hash[position].nil?
            $logger.info "Creating #{name} at #{position}"
            child = world.create(klass, name)
            child.set_position(*position)
            @child_hash[position] = child
        else
            $logger.info "A #{@child_hash[position].class} currently exists at #{position.inspect}"
            nil
        end
    end

    def update(elapsed)
        $logger.info "#{self.class} is updating after #{elapsed} ms"
        @child_hash.each_value do |child|
            $logger.info "[+] Updating #{child.class}"
        end
    end
end

class PlantManager < GenericManager
    # Worldgen methods
    def seed(world)
        $logger.info " [+] Seeding plants"
        @child_types.each do |species|
            $logger.info "  [+] Seeding plant type #{species}"
            (0...species.minimum_population).each do |count|
                rand_x = rand(world.width)
                rand_y = rand(world.height)
                rand_z = world.get_surface(rand_x, rand_y)+1
                create_child(world, species, "#{species}#{count}", [rand_x, rand_y, rand_z])
            end
        end

        $logger.info " [+] PlantManager finished seeding with #{@child_hash.size} entries"
    end
end
