#!/usr/bin/ruby

puts "Reading the project..."
project = File.read("project.pbxproj")
paths = project.scan(/path = [^;]+;/)
mapping = Hash.new

puts "Finding correct paths..."
paths.each do |element|
  element =~ /path = ([^;]+);/
  path = $1
  if path =~ /\.(cpp|h|hpp|m|rb)$/ && !(path =~ /^\./)
    newpath = `find .. | grep /#{path}$`.chomp
    if newpath =~ /\n/
      puts "    Multiple results found for #{path}. Skipping."
    else
      mapping[element] = "name = #{path}; path = #{newpath};"
    end
  end
end

puts "Making text replacements..."
mapping.each do |from, to|
  project.gsub!(from, to)
end

puts "Updated project..."
File.open("Mountainhome.xcodeproj/project.pbxproj", 'w') {|f| f.write(project) }
%x{open Mountainhome.xcodeproj}
puts "Done!"
