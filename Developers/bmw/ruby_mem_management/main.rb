puts "Inside of ruby!"
a = MyClassInRuby.new(10)
a.do(20)
GC.start
puts "Done with ruby"
