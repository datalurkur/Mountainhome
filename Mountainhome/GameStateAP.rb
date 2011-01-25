class GameStateAP
    def self.events
        [
        # Toggles.
        [:toggle_console,   :KEY_BACKQUOTE],
        [:toggle_mouselook, :KEY_TAB],
        # Toggle between wireframe and filled when spacebar is pressed.
        [:toggle_filled,    :KEY_SPACE],

        # Return to MenuState
        [:escape,           :KEY_q],

        # Camera controls
        [:cycle_camera,     :KEY_c],
        [:increase_depth,   :KEY_PAGEDOWN],
        [:decrease_depth,   :KEY_PAGEUP],

        # World interaction / Job menus
        [:open_job_menu,    :KEY_u]
        ]
    end
end