import io
import time

from tests.conf_data import (
    mqtt_listen_period,
)


def check_for_presence(all_messages, expected_messages):
    return set(expected_messages).issubset(set(all_messages))


def generate_file(lines):
    return io.BytesIO("\n".join(lines).encode("utf-8"))


def mqtt_listen(mqtt_client):
    messages = []
    mqtt_client.on_message = lambda client, userdata, msg: messages.append((msg.topic, msg.payload.decode("utf-8")))
    mqtt_client.loop_start()
    time.sleep(mqtt_listen_period)
    mqtt_client.loop_stop()
    return messages
