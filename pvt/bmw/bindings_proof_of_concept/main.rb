puts "Ruby: Inside of main.rb. Defining GameState"

class GameState < State
  def update
    @counter ||= 0

    puts "Ruby: Updating! Counter: #{@counter}"

    if (@counter += 1) > 10
      puts "Ruby: telling engine to stop looping"
      $engine.exit
    end
  end
end

puts "Ruby: Instantiating a GameState object."
state = GameState.new

puts "Ruby: Setting the engine state."
$engine.state = state

puts "Ruby: main.rb is finished!"
