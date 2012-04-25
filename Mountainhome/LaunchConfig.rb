$launch_state = "LoadingState"
$launch_state_args = [:generate, {
    :width => 33, :height => 33, :depth => 9,
    :enable_pathfinding => false,
    :enable_liquid => false,
    :on_launch => Proc.new { |gamestate|
        to_create = [
            [Dwarf, "Franzibald"],
            [Dwarf, "Sheila"]
        ]

        gamestate.shitty_create_actor(to_create)

    }
}]
