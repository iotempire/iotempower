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
* Install ``mosquitto-nossl`` and ``haproxy`` on the OpenWRT router:
  System -> Software
* SSH into the OpenWRT router : ``$ ssh root@<IP of the router OR the hostname>``
* Change content of the haproxy config file to at ``/etc/haproxy.cfg`` to follow the template:

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

    # If only connections from localhost are allowed i.e. only connections over TLS
    listener 1883 127.0.0.1

    # Default port for plain text messages
    #listener 1883

    # Listen on the standard TLS port
    #listener 8883

    # Points to your generated files
    #cafile ca.crt
    #certfile server.crt
    #keyfile server.key

    password_file <path to the configuration file>
    allow_anonymous false