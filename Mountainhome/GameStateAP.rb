class GameStateAP
    # TODO:
    #   Should be modal.
    #   Should handle errors, light when move_right was missnamed 'move_rigt'.
    #   A more human syntax that doesn't rely on symbols (which can also be mistyped) would be nice, too.

    def self.events
        [
        # Toggles.
        [:toggle_console,   :KEY_BACKQUOTE],
        [:toggle_mouselook, :KEY_TAB],
        # Toggle between wireframe and filled when spacebar is pressed.
        [:toggle_filled,    :KEY_SPACE],

        # Toggle various debugging tools
        [:toggle_frustum_culling, :KEY_f],
        [:toggle_bounding_boxes,  :KEY_b],

        # Return to MenuState
        [:quit_to_menu,     :KEY_q],

        # Camera controls
        [:cycle_camera,     :KEY_c],
        [:increase_depth,   :KEY_PAGEDOWN],
        [:decrease_depth,   :KEY_PAGEUP],

        [:move_right,       :KEY_d],
        [:move_left,        :KEY_a],
        [:move_forward,     :KEY_w],
        [:move_backward,    :KEY_s],
        [:move_up,          KeyPressed.new(:KEY_w, :MOD_SHIFT)],
        [:move_down,        KeyPressed.new(:KEY_s, :MOD_SHIFT)],

        [:stop_right,       KeyReleased.new(:KEY_d)],
        [:stop_left,        KeyReleased.new(:KEY_a)],
        [:stop_forward,     KeyReleased.new(:KEY_w)],
        [:stop_backward,    KeyReleased.new(:KEY_s)],
        [:stop_up,          KeyReleased.new(:KEY_w, :MOD_SHIFT)],
        [:stop_down,        KeyReleased.new(:KEY_s, :MOD_SHIFT)],

        # World interaction / Job menus
        [:open_job_menu,    :KEY_u]
        ]
    end
end