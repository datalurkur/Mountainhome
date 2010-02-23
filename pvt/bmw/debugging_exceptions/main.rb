puts "Inside of ruby!"
begin
  nil.hello
rescue Exception => e
  puts "Exception! #{e.message}"
end
puts "Done with ruby"
