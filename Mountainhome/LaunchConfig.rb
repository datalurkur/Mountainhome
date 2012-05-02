$launch_state = "LoadingState"
$launch_state_args = [:generate, {
    :width => 33, :height => 33, :depth => 10,
    :on_launch => Proc.new { |gamestate|
        # Nothing.
    }
}]
