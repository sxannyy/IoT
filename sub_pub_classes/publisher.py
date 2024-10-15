import time
import paho.mqtt.client as mqtt_client
import random
import requests
from loguru import logger
from uuid import getnode as get_mac
import serial
import time

logger.add('logs/logs.log')

broker="broker.emqx.io"

responses = {'d': 7,  # "led_off" 
             'u': 6,  # "led_on" 
             'p': 3}  # 0 -> 1023 zero fill to leftside
port_photo = "/dev/ttyUSB0" # "COM4" or similar for windows
connection_photo = serial.Serial(port_photo, baudrate=9600, timeout=0.1) # baudrate=9600

try:
    unique_id = requests.get('http://172.20.10.3:8000/get_id').json()[0]
    logger.info("Уникальный id выдан успешно")
except requests.exceptions.ConnectionError:
    logger.error("Сервер не запущен")

client = mqtt_client.Client(
   mqtt_client.CallbackAPIVersion.VERSION1,
   unique_id
)

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    str_resp: str = ""
    connection.write(cmd.encode())
    
    if response_len > 0:
        resp = connection.read(response_len)
        str_resp = resp.decode()
    return str_resp

print("Connecting to broker",broker)
print(client.connect(broker))
client.loop_start() 
print("Publishing")

req = requests.get('http://172.20.10.3:8000/get_topic').json()[0]

while True:
    try:
        photo_val_resp: str = send_command('p', responses['p'], connection_photo)
        photo_val = int(photo_val_resp)
        print(photo_val)
    except Exception as e:
        print(e)
        continue
    state = "on" if photo_val > 50 else "off"
    print(f"state is {state}")
    client.publish(req, state)
    logger.info("Значение с фоторезистора равно - " + str(photo_val))
    time.sleep(2)
    
client.disconnect()
client.loop_stop()
