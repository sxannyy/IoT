import time
import paho.mqtt.client as mqtt_client
import requests
from loguru import logger
import serial
from collections import deque

logger.add('logs/logs.log')

broker = "broker.emqx.io"
responses = {
    'd': 7,
    'u': 6,
    'p': 3
}

port_photo = "/dev/ttyUSB0"
connection_photo = serial.Serial(port_photo, baudrate=9600, timeout=0.1)

try:
    unique_id = requests.get('http://192.168.0.104:8080/get_unique_id').json()[0]
    server_id = requests.get('http://192.168.0.104:8080/get_server_id').json()[0]
    print(unique_id)
    print(server_id)
    logger.info("Уникальный id выдан успешно")
except requests.exceptions.ConnectionError:
    logger.error("Сервер не запущен")
    exit(-1)

client = mqtt_client.Client(client_id=unique_id)

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    connection.write(cmd.encode())
    if response_len > 0:
        return connection.read(response_len).decode()
    return ""

stream_active = True
min_brightness = float('inf')
max_brightness = float('-inf')

def on_message(client, userdata, message):
    global stream_active
    command = message.payload.decode("utf-8")
    if command == "on":
        stream_active = True
        logger.info("Стрим включен.")
    elif command == "off":
        stream_active = False
        logger.info("Стрим выключен.")

client.on_message = on_message

print("Подключение к брокеру", broker)
client.connect(broker)
client.loop_start()

client.subscribe(f"lab/{server_id}/photo/activate_stream")
req_instant = f"lab/{server_id}/photo/instant"
req_average = f"lab/{server_id}/photo/average"
req_stream = f"lab/{server_id}/photo/stream"
req_min = f"lab/{server_id}/photo/min"
req_max = f"lab/{server_id}/photo/max"

value_queue = deque(maxlen=100)
sum_values = 0

instant_interval = 0.5
last_instant_time = time.time()

average_interval = 0.5
last_average_time = time.time()

stream_interval = 0.5
last_stream_time = time.time()

min_max_interval = 5.0
last_min_max_time = time.time()

while True:
    current_time = time.time()

    try:
        photo_val_resp = send_command('p', responses['p'], connection_photo)
        photo_val = int(photo_val_resp)
    except Exception as e:
        logger.error(f"Ошибка считывания: {e}")
        continue

    if current_time - last_instant_time >= instant_interval:
        last_instant_time = current_time
        client.publish(req_instant, f"{photo_val}")
        logger.info(f"Мгновенное значение: {photo_val}")

    if current_time - last_average_time >= average_interval:
        last_average_time = current_time
        if len(value_queue) == 100:
            sum_values -= value_queue[0]
        value_queue.append(photo_val)
        sum_values += photo_val

        if len(value_queue) > 0:
            moving_average = sum_values / len(value_queue)
            client.publish(req_average, f"{moving_average}")
            logger.info(f"Бегущее среднее: {moving_average}")

    if stream_active and (current_time - last_stream_time >= stream_interval):
        last_stream_time = current_time
        client.publish(req_stream, f"{photo_val}")
        logger.info(f"Текущее значение: {photo_val}")

    if current_time - last_min_max_time >= min_max_interval:
        last_min_max_time = current_time
        min_brightness = min(min_brightness, photo_val)
        max_brightness = max(max_brightness, photo_val)
        client.publish(req_min, f"{min_brightness}")
        client.publish(req_max, f"{max_brightness}")
        logger.info(f"Мин: {min_brightness}, Макс: {max_brightness}")

client.disconnect()
client.loop_stop()
