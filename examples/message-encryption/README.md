# Application-Layer Encryption Proof of Concept

This is a Proof of Concept application-layer encryption implementation.

The files have to be manually added to the build folder and Node-RED in order for this PoC to work. Currently no further integration with IoTempower is done, as the TLS is preferred for security.

This setup uses a counter saved to EEPROM to mitigate message replay attacks. An example of both sending and receiving an encrypted message is provided. The example uses a simple button node for sending the encrypted message and a LED node for receiving the messages. These nodes have ample documentation available for new users to explore so no further explanation is provided here.

### Files to be added to the build folder:

* Move `aes_helper.h` to `build/src/aes_helper.h`

### Modifications for existing files:

* Add the following line: `#include <aes_helper.h>` to `build/src/setup.cpp`
* Move:
  * `receive/setup.h` to `build/src/setup.h` of the LED node
  * `send/setup.h` to `build/src/setup.h` of the BUTTON node

### Files to be added to Node-RED (As the content of a function node):

* `Node-RED_decryption_node`
* `Node-RED_encryption_node`