class GameStateAP
    # TODO:
    #   Should be modal.
    #   Should handle errors, light when move_right was missnamed 'move_rigt'.
    #   A more human syntax that doesn't rely on symbols (which can also be mistyped) would be nice, too.

    def self.events
        [
        #:key or [:key,:mod],  :on_press, :on_release

        # Toggle various debugging tools
        [:KEY_f, :toggle_frustum_culling],
        [:KEY_b, :toggle_bounding_boxes],
        [:KEY_v, :toggle_path_visualizer],
        [:KEY_p, :toggle_paused],

        # Toggle mouse-look or mouse.
        [:KEY_TAB,       :toggle_mouselook],
        # Toggle the console.
        [:KEY_BACKQUOTE, :toggle_console],
        # Toggle between wireframe and filled when spacebar is pressed.
        [:KEY_SPACE,     :toggle_filled],


        # Return to MenuState
        [:KEY_q, :quit_to_menu],

        # Camera controls
        [:KEY_c,        :cycle_camera],
        [:KEY_PAGEDOWN, :increase_depth],
        [:KEY_PAGEUP,   :decrease_depth],

        # Menus
        [:KEY_u, :open_job_menu],

        # Movement.
        [:KEY_w,               :move_forward,  :stop_forward],
        [:KEY_a,               :move_left,     :stop_left],
        [:KEY_s,               :move_backward, :stop_backward],
        [:KEY_d,               :move_right,    :stop_right],
        [[:KEY_w, :MOD_SHIFT], :move_up,       :stop_up  ],
        [[:KEY_s, :MOD_SHIFT], :move_down,     :stop_down],
        ]
    end
end
