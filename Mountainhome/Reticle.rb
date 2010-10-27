class Reticle < MHSelection
#    def initialize(name, manager, x, y, w, h, args={})
#        super(name, manager, "blue", "", "", args)
#        set_dimensions(x, y, w, h)
#    end
#    def initialize(world)
    attr_accessor :position

    def initialize(world)
        @world = world
        @position = [1,1, @world.terrain.get_surface(1,1)]
        @entity = @world.create_entity("reticle", "Dwarf", "blue-lit", @position[0], @position[1], @position[2] )
    end

    def input_event(event)
        # move normally using ijkl
        return :unhandled
        
        if event.key ==  Keyboard.KEY_h and Event.KeyPressed
            @position[1] += 1
            @position[3] = @world.terrain.get_surface(@position[0], @position[1])
            @entity.set_position(*@position)
        end
        # select with <enter>
        # extend with 
    end
end
