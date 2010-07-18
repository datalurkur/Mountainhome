module Moveable
    attr_accessor :path

    def random_move
        randxy = [rand(self.world.terrain.width), rand(self.world.terrain.height)]
        self.path = self.world.find_path(self.position[0], self.position[1], randxy[0], randxy[1])
    end

    def move(x, y)
        self.path = self.world.find_path(self.position[0], self.position[1], x, y)
    end

    # this shouldn't be in a module
    def update(elapsed)
        if self.path
            next_step = self.path.next_step
            $logger.info "Moving #{self.inspect} to #{next_step.inspect}"
            self.entity.set_position(*next_step)
            self.position = next_step
            
            if self.path.end_of_path?
                $logger.info "EOP for #{self.inspect}; generating new random path"
                self.path = nil
                random_move
            end
        end
    end
end

module Mine
    def mine
        # Eventually we'll want to make a JobManager of some sort that examines current jobs
        # and available dwarves and their selected tasks, and distributes jobs appropriately.

        # for now:
        # 1. randomly move to tile
        # 2. 

        # FULL PATH
        # 1. pick random exposed tile to mine
        # 2. find tile next to it to move to
        # 3. pathfind to 2's tile
        # 4. destroy tile
        # 5. goto 1
    end
end