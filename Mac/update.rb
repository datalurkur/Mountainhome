#!/usr/bin/ruby
require 'fileutils'
require 'pp'

puts "Making backup..."
FileUtils.cp("Mountainhome.xcodeproj/project.pbxproj", "project.pbxproj")

puts "Reading the project..."
project = File.read("project.pbxproj")
paths = project.scan(/(path = ([^;]+);)/)

files = `find ..`.split("\n").select { |file| file =~ /^\.\.\/(engine|base|mountainhome|render|content)/i }

mapping = Hash.new

puts "Finding correct paths..."
paths.each do |element, path|
  # FIXME: I'm not sure why the 'path =~ /^\.\./' part is here, but it means that things under Mac don't get fixed D: Tragically, removing it seems to break things.
  if !File.exists?(path) && path =~ /^\.\./
    matches = files.select { |file| file =~ /\/#{File.basename(path)}$/ }

    if matches.size > 1
      puts "    Skipping #{path}. Multiple results found."
    elsif matches.size == 0
      puts "    Skipping #{path}. No results found."
    else
      if path != matches[0]
        mapping[element] = "path = #{matches[0]};"
        puts "    Updated needed for #{path} => #{matches[0]}"
      else
        raise RunetimeError, "This case shouldn't hit, I don't think."
      end
    end
  end
end

if !ARGV.include?("-p")
  puts "Making text replacements..."
  mapping.each do |from, to|
    project.gsub!(from, to)
  end

  puts "Updated project..."
  File.open("Mountainhome.xcodeproj/project.pbxproj", 'w') { |f| f.write(project) }
  %x{open Mountainhome.xcodeproj}
  puts "Done!"
end
