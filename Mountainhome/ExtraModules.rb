require 'Path'

module Moveable
    attr_accessor :path, :eom_action

    def position; @position || [0,0,0]; end

    def position=(value)
        self.set_position(*value)
        @position = value
    end

    def move_random
        x = rand(self.world.terrain.width)
        y = rand(self.world.terrain.height)
        z = self.world.terrain.get_surface(x, y)+1
        self.move([[x,y,z]])
    end

    # Take an array of locations, trying to find a path to each one,
    #  using the first successful query as the destination
    def move(locations, &block)
        locations.each do |loc|
            self.path = Path.new(self.world, self.position, loc)
            break if !self.path.blocked?
        end

        if block_given?
            self.eom_action = block
        end

        return true
    end

    # this shouldn't be in a module
    def update(elapsed)
        if !self.path.nil?
            if self.path.blocked?
                $logger.info "Path blocked, EOM action cancelled"
                self.path = nil
                self.eom_action = nil
            elsif self.path.end_of_path?
                $logger.info "End of path reached for actor #{self.name}"
                self.path = nil

                if !self.eom_action.nil?
                    $logger.info "Executing EOM action"
                    self.eom_action.call
                    # The block is responsible for clearing the EOM action
                end
            else
                next_step = self.path.next_step
                self.position = next_step
            end
        elsif self.eom_action
            $logger.info "Path not found, EOM action cancelled"
            self.eom_action = nil
        end
    end
end

module Mining
    include Moveable

    def mine_random
        (0..100).each {
            randxyz = [rand(self.world.terrain.width), rand(self.world.terrain.height)]
            randxyz << self.world.terrain.get_surface(randxyz[0], randxyz[1])

            if randxyz[2] > 0
                self.mine(*randxyz)
                return
            end
        }
    end

    # This will eventually be a part of some more generic job-oriented module
    def access_points(x, y, z)
        # Enumerate the locations a tile can be accessed from
        possibilities = [
            # For now, just the same z-level or above in cardinal directions
            [x+1, y, z],   [x, y+1, z],   [x-1, y, z],   [x, y-1, z],
            [x+1, y, z+1], [x, y+1, z+1], [x-1, y, z+1], [x, y-1, z+1]
        ]

        possibilities.reject! { |poss|
            poss[0] < 0 || poss[0] >= self.world.terrain.width ||
            poss[1] < 0 || poss[1] >= self.world.terrain.height ||
            poss[2] < 0 || poss[2] >= self.world.terrain.depth ||
            self.world.terrain.get_tile_type(*poss) != 0
        }

        possibilities
    end

    def mine(x, y, z)
        # Eventually we'll want to make a JobManager of some sort that examines current jobs
        # and available dwarves and their selected tasks, and distributes jobs appropriately.

        # Pick an access tile, for now pick the first for simplicity
        access = access_points(x,y,z)

        if access.size == 0
            $logger.info "No access to tile at #{[x,y,z].inspect}"
        else
            $logger.info "Attempting to mine tile at #{[x,y,z].inspect}"
            self.move(access) {
                $logger.info "Mining tile at #{[x,y,z].inspect}"
                self.world.terrain.set_tile_type(x, y, z, 0)

                # For now, go ahead and pick a new spot to mine
                self.mine_random
            }
        end
    end
end
