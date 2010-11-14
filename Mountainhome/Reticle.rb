# Make the reticle float a tiny bit above the tile it's hovering over.
Z_OFFSET = 0.1
# Compensate for Entity adding 0.5 to its position.
POSITION_OFFSET = 0.5

class Reticle < MHSelection
    def initialize(world)
        @world = world
        @entity = @world.create(MHEntity, "reticle", "Quad", "blue-lit")
        @entity.set_position(1.5, 1.5, @world.terrain.get_surface(1,1) + Z_OFFSET)
    end

    def input_event(event)
        # move normally using ijkl
        if event.is_a?(KeyPressed)
            case event.key
            when Keyboard.KEY_h
                shift_position(:x, -1)
                return :handled
            when Keyboard.KEY_j
                shift_position(:y, -1)
                return :handled
            when Keyboard.KEY_k
                shift_position(:y, 1)
                return :handled
            when Keyboard.KEY_l
                shift_position(:x, 1)
                return :handled
            end
        end
        # select with <enter>
        # extend with 
        return :unhandled
    end

    private
    # Move entity's position around, staying on top of the world for now.
    def shift_position(index, amount = 1)
        pos = in_world_coords(@entity.position)
        $logger.info("pos was #{pos.inspect}")

        case index
        when :x, 0, :width
            pos[0] = [[pos[0] + amount, 0].max,  @world.width - 1].min
        when :y, 0, :height
            pos[1] = [[pos[1] + amount, 0].max, @world.height - 1].min
        end
        # stay on top of the world for now
        pos[2] = @world.terrain.get_surface(pos[0], pos[1]) + Z_OFFSET
        $logger.info("pos is #{pos.inspect}")

        pos = in_entity_coords(pos)
        @entity.set_position(pos[0], pos[1], pos[2])
    end

    # Entity currently adds 0.5 to the x,y position parameters passed to create_entity.
    # This means that we have to translate by this much when interacting with world.
    def  in_world_coords(pos) translate_coords(pos, :-); end
    def in_entity_coords(pos) translate_coords(pos, :+); end
    def translate_coords(pos, direction)
        pos[0] = pos[0].send(direction, POSITION_OFFSET)
        pos[1] = pos[1].send(direction, POSITION_OFFSET)
        pos
    end
end
