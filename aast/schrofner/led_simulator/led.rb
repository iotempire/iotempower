#! /usr/bin/env ruby

require 'rubygems'
require 'mqtt'
require 'paint'

control_key = "/turn"
status = 1 #0 = off, 1 = on
color = "red" #last set color

input_array = ARGV

def color_window(window_color)
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

  puts Paint[fill_string, window_color, window_color]
end

if input_array.length == 2
  color_window(color)
  MQTT::Client.connect(input_array[0]) do |c|
    c.subscribe(input_array[1])
    c.subscribe(input_array[1] + control_key)

    c.get() do |topic,message|
      if topic == input_array[1] + control_key
        if message == "off"
          status = 0
          color_window("black")
        elsif message == "on"
          status = 1
          color_window(color)
        end
      else
        color = message
        if status == 1
          color_window(message)
        else
          color_window("black")
        end
      end

    end
  end
else
  puts 'not enough arguments! need server and topic'
end
