$launch_state = "LoadingState"
$launch_state_args = [:generate, {
    :width => 17, :height => 17, :depth => 9,
    :start_paused => true,
    :on_launch => Proc.new { |gamestate|
        # This is just a simple example; technically it's redundant to start paused and then unpause,
        #  but I put this here to demonstrate the control you have over this.
        gamestate.run "ExampleTest"
        gamestate.toggle_paused
    }
}]
