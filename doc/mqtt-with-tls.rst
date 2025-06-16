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
This generates the necessary certifiFcates and keys in the folder specified by the
``IOTEMPOWER_MQTT_CERT_FOLDER`` environment variable or, if not set, in the current active folder.


Using Your Own Certificates
====================================

If you already have your own certificates, you can use them instead of generating new ones.
To do this, define the ``IOTEMPOWER_MQTT_CERT_FOLDER`` environment variable to point to the folder
where your certificates are stored. The folder should definitely store a ``ca.crt`` file, which is the CA certificate.
You also need to ensure that the CA certificate, server certificate and key are correctly configured in your MQTT broker.
If using the IoTempower MQTT broker, it will automatically use the certificates from the specified folder.
In this case the certificate folder should also contain ``server.crt`` and ``server.key`` files, which are the server certificate and key respectively.
