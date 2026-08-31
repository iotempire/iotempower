import paho.mqtt.publish as publish

def publish_message(topic, message, ip):
    publish.single(
        topic=topic,
        payload=message,
        hostname=ip
    )