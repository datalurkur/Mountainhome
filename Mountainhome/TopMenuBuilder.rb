require 'UIManager'

class TopMenuBuilder
    def self.create_top_menus(manager, state)
        center = [manager.width/2, manager.height/2]
        manager.clear_elements

        creation_proc = Proc.new do
            state.create_world
        end

        Button.new("create", manager, "Create New World", center[0], center[1], 200, 50, creation_proc)
    end
end

