#!../ruby/bin/ruby

require 'MountainHomeDSL.rb'

class MountainhomeWorld < MHWorld
  def initialize(params={})
    @height = params[:height] || 33
    @width  = params[:width]  || 33
    @depth  = params[:depth]  || 17
    super(@width, @height, @depth)
  end # def initialize
end
