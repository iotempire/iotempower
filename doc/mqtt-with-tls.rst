==========
Using MQTT via TLS
==========

By default, IoTempower uses MQTT without TLS. However, you can
use MQTT with TLS for secure communication. To do this, you need to
generate certificates, configure your MQTT broker accordingly and 
then use the certificates in your IoTempower system by defining the ``IOTEMPOWER_MQTT_CERT_FOLDER`` 
variable and setting ``IOTEMPOWER_MQTT_USE_TLS`` variable to ``1`` in the ``system.conf`` file.

Using the IoTempower MQTT Broker
====================================

If using the IoTempower's included MQTT broker (i.e. with ``mqtt_starter``), then
if run in a system folder, where the ``IOTEMPOWER_MQTT_USE_TLS`` is set to ``1``, 
the broker will automatically use the certificates from the ``IOTEMPOWER_MQTT_CERT_FOLDER``
environment variable and expose port ``8883`` for secure MQTT communication.
In this mode, the included broker does not expose the plaintext MQTT listener on port ``1883``.
The ``mqtt_listen`` and ``mqtt_send`` commands will also use the same port and certificate, if the variables are set.


Generating Certificates
====================================

To enable MQTT with TLS, you need to generate a CA certificate and a server certificate.
You can do this using the ``mqtt_generate_certificates`` script provided by IoTempower.
This generates the necessary certificates and keys in the folder specified by the
``IOTEMPOWER_MQTT_CERT_FOLDER`` environment variable or, if not set, in the current active folder.


Using Your Own Certificates
====================================

If you already have your own certificates, you can use them instead of generating new ones.
To do this, define the ``IOTEMPOWER_MQTT_CERT_FOLDER`` environment variable to point to the folder
where your certificates are stored. The folder should definitely store a ``ca.crt`` file, which is the CA certificate.
You also need to ensure that the CA certificate, server certificate and key are correctly configured in your MQTT broker.
If using the IoTempower MQTT broker, it will automatically use the certificates from the specified folder.
In this case, the certificate folder should also contain ``server.crt`` and ``server.key`` files, which are the server certificate and key, respectively.

Using the OpenWRT router with Mosquitto and Haproxy
====================================

To use the OpenWRT router as the MQTT broker, the following steps are needed:

* Give the OpenWRT router a hostname and add it using a DNS entry:
  Network -> DHCP and DNS -> DNS Records -> Add
* Set the ``IOTEMPOWER_MQTT_HOST`` variable to the hostname, not the IP.
* Generate the certificates using ``mqtt_generate_certificates``.
* Install ``mosquitto`` and ``haproxy`` on the OpenWRT router:
  System -> Software
* SSH into the OpenWRT router : ``$ ssh root@<IP of the router OR the hostname>``
* Change content of the HAProxy config file at ``/etc/haproxy.cfg`` to follow the template:

Under Global parameters add:

.. code-block:: text

    tune.ssl.maxrecord 512 # Buffer sizes, set it to the same size as defined in main.cpp

    ssl-default-bind-ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305

    ssl-default-bind-ciphersuites TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256

    ssl-default-bind-options ssl-min-ver TLSv1.2 no-tls-tickets

At the end of the file add two new groups:

.. code-block:: text

    frontend mqtt_tls
        # Listen on port 8883 using the combined pem file
        bind *:8883 ssl crt /etc/mosquitto/mqtt_cert.pem
        mode tcp
        option tcplog
        default_backend mosquitto_local

    backend mosquitto_local
        mode tcp
        # Forward the decrypted traffic to local Mosquitto
        server mosquitto 127.0.0.1:1883

Create the file ``/etc/mosquitto/mqtt_cert.pem`` and copy the contents of ``server.crt`` and ``server.key`` from the ``/certs`` folder in the following format:

.. code-block:: text

    <server.crt content>
    <server.key content>

Restart haproxy with ``/etc/init.d/haproxy restart``

Mosquitto supports TLS, but OpenWrt builds do not expose fine-grained TLS record size control. HAProxy is used here to enforce smaller TLS records ``tune.ssl.maxrecord`` for constrained devices.

Adding client verification with Mosquitto authentication
====================================


To authenticate the client as well as the server, you can use Mosquitto authentication:
https://mosquitto.org/documentation/authentication-methods/

Quick setup guide:

* Create a password file: ``mosquitto_passwd -c <password file> <username>``
* Edit the ``mosquitto.conf`` file and add the following right after the listener line:

.. code-block:: text
    # Plaintext listener for internal traffic (HAProxy traffic)
    # If only connections from localhost are allowed i.e. only connections over TLS

    listener 1883 127.0.0.1

    # Do not allow unauthenticated connections

    allow_anonymous false

    # TLS traffic

    # Listen on the standard TLS port
    listener 8883

    # Points to your generated files
    
    certfile server.crt
    keyfile server.key
    
    # Define allowed cipher suites. Can be optained via [openssl ciphers] command                                                                     

    ciphers ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305
    ciphers_tls1.3 TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256:TLS_AES_128_GCM_SHA256

    password_file <path to the configuration file>
    allow_anonymous false


OpenWRT 25.12.2^ Mosquitto
====================================

OpenWRT 25.12.2 supports Mosquitto versio 2.0.22, which supports Maximum Fragment Length Negotiation (MFLN) needed for TLS with ESP8266 devices and other devices with constrained memory.

To setup Mosquitto on the OpenWRT, some additional configuration is needed.

* SSH into the OpenWRT router : ``$ ssh root@<IP of the router OR the hostname>``
* Change the content of the Mosquitto config file at ``/etc/config/mosquitto`` to follow the template:

.. code-block:: text

    config owrt 'owrt'
        option use_uci '0'

    config mosquitto 'mosquitto'
            option config_file '/etc/mosquitto/mosquitto.conf'

    config persistence 'persistence'

* Create the certificates with ``mqtt_generate_certificates``
* Copy ``server.crt`` and ``server.key`` files to ``etc/mosquitto/`` folder.
* Use the ``mosquitto.conf`` file template provided eariler or create a new one in the ``/etc/mosquitto/`` folder. 
* Restart the Mosquitto service : ``# service mosquitto restart``
* Verify that the desired listeners are in place on ports 1883/8883 with ``# netstat -tlpn``


OpenWRT configuration help
====================================

To use the OpenWRT router as the MQTT broker, some additional configuration is required.

* Set up the network:
Network -> Wireless -> enable, edit -> Set the encryption method to something secure like WPA2-PSK, create a strong passphrase.
* Allow NTP servers:
To have the router act as the Network Time Protocol (NTP) server without being connected to the Internet, the next steps should be made:
System -> Time synchronization -> Provide NTP server
Bind the NTP server to LAN.
* Bind a hostname to the router (Guide under ``Using the OpenWRT router with Mosquitto and Haproxy`` section)
As the NTP internal service starts a listener on ``:::123`` (IPv6), it is easier to query the time using a hostname. IoTempower will automatically use the hostname from the ``IOTEMPOWER_MQTT_HOST`` variable.
