class GenericManager
    def initialize
        @child_hash = Hash.new
        @child_types = Array.new
    end

    def register(klass)

        if @child_types.include? klass
            $logger.info "#{klass} has already been registered with #{self.class}"
        else
            $logger.info "Registering #{klass} with #{self.class}"
            @child_types << klass
        end
    end

    def create_child(klass, position)
        if @child_hash[position].nil?
            $logger.info "Creating a #{klass.class} at #{position.inspect}"
            @child_hash[position] = klass.new
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
    def seed
        $logger.info " [+] Seeding plants"
        @child_types.each do |species|
            $logger.info "  [+] Seeding plant type #{species}"
        end
    end
end
