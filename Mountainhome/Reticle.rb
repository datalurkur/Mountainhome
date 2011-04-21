class Reticle < MHSelection
    #include TranslatePosition

    def initialize(world)
        @world = world
        #@entity = @world.create(MHEntity, "reticle", "Quad", "blue-lit")

        # Float slightly above the active tile.
        @z_offset = 0.1
        #self.position = [1, 1, @world.get_surface_level(1,1) + 1]
        Event.add_listener(self)
    end

    def input_event(event)
        # move normally using hjkl
        if event.is_a?(KeyPressed)
            case event.key
            when Keyboard.KEY_h
                self.x = self.x - 1 unless self.x == 0
            when Keyboard.KEY_j
                self.y = self.y - 1 unless self.y == 0
            when Keyboard.KEY_k
                self.y = self.y + 1 unless self.y + 1 == @world.height
            when Keyboard.KEY_l
                self.x = self.x + 1 unless self.x + 1 == @world.width
            else return :unhandled
            end
            self.z = @world.get_surface_level(self.x, self.y) + 1
            return :handled
        end
        # select with <enter>
        # extend with 
        return :unhandled
    end
end
