#! /usr/bin/env ruby

require 'rubygems'
require 'mqtt'
require 'paint'

input_array = ARGV

if input_array.length == 2
  MQTT::Client.connect(input_array[0]) do |c|
    c.subscribe(input_array[1])
    c.get() do |topic,message|
      system('clear') 
      term_width = `/usr/bin/env tput cols`.to_i
      term_height = `/usr/bin/env tput lines`.to_i
      fill_string = ""

      for i in 1..term_height
        for j in 1..term_width
          fill_string = fill_string + "#"
        end
        fill_string = fill_string + "\n"
      end

      puts Paint[fill_string, message, message]
    end
  end
else
  puts 'not enough arguments! need server and topic'
end
