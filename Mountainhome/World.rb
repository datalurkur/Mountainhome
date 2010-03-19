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
    @pitch = 0
    @yaw = 0
  end # def initialize

  def update(elapsed)
      sens_cap = 0.05
      pitch = [[@pitch*elapsed, sens_cap].min, -sens_cap].max
      yaw =   [[@yaw*elapsed,   sens_cap].min, -sens_cap].max
      @camera.rotate_on_axis(pitch, 1, 0, 0) if @pitch != 0.0
      @camera.rotate_on_axis(yaw, 0, 0, 1) if @yaw != 0.0
      @pitch = 0
      @yaw = 0
  end

  def input_event(params={})
    case params[:type]
    when :move
      rotate_speed = -0.001
      @yaw = params[:x] * rotate_speed
      @pitch = params[:y] * rotate_speed
      return :handled
    else
      return :unhandled
    end
  end
end
