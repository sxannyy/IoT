import time
import paho.mqtt.client as mqtt_client
from uuid import getnode as get_mac
import hashlib

BROKER = "broker.emqx.io"
TOPIC = "lab/esp8266/laba_lamp"

h = hashlib.new('sha256')
mac = get_mac()
h.update(str(mac).encode())
pub_id = h.hexdigest()[:10]

client = mqtt_client.Client(pub_id)

print("Connecting to broker", BROKER)
client.connect(BROKER, 1883)
client.loop_start()
print("Publishing")

is_on = False
cycle_duration = 60
on_time_start = 20
on_time_end = 40

while True:
    cur_time = time.time() % cycle_duration

    if on_time_start <= cur_time < on_time_end:
        if not is_on:
            client.publish(TOPIC, "led__on")
            is_on = True
    else:
        if is_on:
            client.publish(TOPIC, "led_off")
            is_on = False

    time.sleep(0.1)

client.disconnect()
client.loop_stop()
