IRSender
==========

```
IRSender(name, nec, signalPort);
```
Create a new IR Sender to send IR signals.

## Parameters

- name: the name it can be addressed via MQTT in the network.
- nec: the nec code that will be sent.
- signalPort: GPIO pin.

## Example

```
IRSender(IRSend, 0x68B92 ,D2);
```

This example will enable an IR sender module to send IR signals.

## Wiring

This is an example of wiring a KY-005 IR Sender.

| KY-005  | Board |
| ------------- | ------------- |
| S  | GPIO  |
| Middle  | 3V3 or 5V*  |
| -  | GND  |

*depending on the board
