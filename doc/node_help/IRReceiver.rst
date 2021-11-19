IRReceiver
==========

```
IRReceiver(name, signalPort);
```
Create a new IR Receiver to receive IR signals.

## Parameters

- name: the name it can be addressed via MQTT in the network.
- signalPort: GPIO pin

## Example

```
IRReceiver(IRRemote, D5);
```

This example will enable an IR receiver module to receive IR signals.

## Wiring

WARNING: Most documented examples show that IR receivers should receive 5V on their Power Line, but that is not the case with some boards (ex. Wemos D1 Mini). 

This is an example of wiring a KY-022 IR Receiver.

| KY-022  | Board |
| ------------- | ------------- |
| S  | GPIO  |
| Middle  | 3V3 or 5V*  |
| -  | GND  |

*depending on the board
