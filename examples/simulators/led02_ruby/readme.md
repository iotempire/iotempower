# LED Simluator
This is a MQTT LED simulator written in Ruby.
To launch it, simply use the following call:

```bash
./led.rb MQTT_SERVER CHANNEL
```

Where MQTT_SERVER is the IP address of the MQTT server and CHANNEL is the channel to subscribe to.

An example call would be: `./led.rb "192.168.12.1" "led/1"`

## Installing the dependencies
If the dependencies are not installed already, make sure you have [bundler](https://bundler.io/) installed. Then call `bundler install` inside the directory. This will automatically pull all the dependencies from rubygem.

## Controlling the simulator
### Setting the color
The LED color can be set via the following endpoint: /rgb/set
It needs to be appended to the channel, which was defined when launching the simulator.

Examples:
`led/1/rgb/set green`
`led/1/rgb/set yellow`
`led/1/rgb/set #ffffff`
`led/1/rgb/set #000000`

### Turning on and off the LED
The LED can also be turned on and off via the endpoint: /set
This also needs to be appended to the channel when launching the simulator.

Examples:
`led/1/set on`
`led/1/set off`

## Reading the current status
Whenever the status of the LED has changed, it will publish its current state and color on the following channels: default channel, /rgb/status.

Examples:
`led/1 on`
`led/1/rgb/status green`
