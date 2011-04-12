class GameStateAP
    # TODO:
    #   Should be modal.
    #   Should handle errors, light when move_right was missnamed 'move_rigt'.
    #   A more human syntax that doesn't rely on symbols (which can also be mistyped) would be nice, too.

    def self.events
        [
        #:name,             :key or event
        #:key, or [:key,:mod]  :on_press, :on_release
        [:KEY_BACKQUOTE, :toggle_console],
        [:KEY_TAB, :toggle_mouselook],
        # Toggle between wireframe and filled when spacebar is pressed.
        [:KEY_SPACE,    :toggle_filled],

        # Toggle various debugging tools
        [:KEY_f, :toggle_frustum_culling],
        [:KEY_b,  :toggle_bounding_boxes],

        # Return to MenuState
        [:KEY_q, :quit_to_menu],

        # Camera controls
        [:KEY_c, :cycle_camera],
        [:KEY_PAGEDOWN,   :increase_depth],
        [:KEY_PAGEUP,   :decrease_depth],

        # Menus
        [:KEY_u, :open_job_menu],

        # Movement.
        [:KEY_w,     :move_forward,  :stop_forward],
        [:KEY_a,        :move_left,     :stop_left],
        [:KEY_s,    :move_backward, :stop_backward],
        [:KEY_d,       :move_right,    :stop_right],
        [[:KEY_w, :MOD_SHIFT], :move_up,   :stop_up  ],
        [[:KEY_s, :MOD_SHIFT], :move_down, :stop_down],

#        [KeyReleased.new(:KEY_d), :stop_right],
#        [:stop_left,        KeyReleased.new(:KEY_a)],
#        [:stop_forward,     KeyReleased.new(:KEY_w)],
#        [:stop_backward,    KeyReleased.new(:KEY_s)],
#        [:stop_up,          KeyReleased.new(:KEY_w, :MOD_SHIFT)],
#        [:stop_down,        KeyReleased.new(:KEY_s, :MOD_SHIFT)],
        ]
    end
end