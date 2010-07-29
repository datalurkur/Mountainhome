class PlantManager
    @plant_hash = Hash.new

    def register(klass)
        $logger.info "Registering PlantManager"
    end

    # Primitive methods
    def create_plant(klass, position)
        if @plant_hash[position].nil?
            $logger.info "Creating a #{klass.class} at #{position.inspect}"
            @plant_hash[position] = klass.new
        else
            $logger.info "A #{@plant_hash[position].class} currently exists at #{position.inspect}"
        end
    end

    # Worldgen methods
    def seed_plants
    end

    # Game methods
    def update(elapsed)
        $logger.info "PlantManager is updating after #{elapsed} ms"
        @plant_hash.each_value do |plant|
            $logger.info "[+] Updating #{plant.class}"
        end
    end
end
