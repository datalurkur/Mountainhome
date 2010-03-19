require 'Camera'

class World < MHWorld
  def initialize(params={})
    @height = params[:height] || 33
    @width  = params[:width]  || 33
    @depth  = params[:depth]  || 17
    super(@width, @height, @depth)
	
	# Create a default camera
	@camera = Camera.new(self, 0)
    @camera.set_fixed_yaw(0, 0, 1)
    @camera.set_position(0.25*@width, 0.25*@height, 3*@depth)
    @camera.look_at(0.55*@width, 0.45*@height, 0)
  end # def initialize
end
