require 'Test.bundle'
require 'logger'

logger = Logger.new(STDOUT)
t = Test.new(logger)
puts t.add("Bill Chase")