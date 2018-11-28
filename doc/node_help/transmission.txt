transmission(transmission_interval=5)

Set the status report transmission interval (must be placed interval
start method, see node-running-test example).

Parameter
- transmission_interval: send every transmission_interval seconds a complete
  status update. If transmission_interval=0, send never status updates.

example:
transmission(10)
