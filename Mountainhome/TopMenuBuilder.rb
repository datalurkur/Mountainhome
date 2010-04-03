require 'UIManager'

class TopMenuBuilder
    def self.create_top_menus(manager, state)
        center = [manager.width/2, manager.height/2]
        manager.clear_elements

        creation_proc = Proc.new do
            state.create_world
        end

        manager.add_element("B_create_world", center[0]-100, manager.height-200, 300, 50,
                            {:mat => "t_grey", :text => "Create New World",
                             :element_type => Clickable, :click_proc => creation_proc})
    end
end

