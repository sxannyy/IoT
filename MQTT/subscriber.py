import time
import paho.mqtt.client as mqtt_client
import requests
from loguru import logger
import serial

logger.add('logs/logs.log')

broker = "broker.emqx.io"
responses = {
    'd': 7,
    'u': 6,
    'p': 3
}
port_led = "/dev/ttyUSB0"
connection_led = serial.Serial(port_led, baudrate=9600, timeout=0.1)

try:
    unique_id = requests.get('http://192.168.0.104:8080/get_unique_id').json()[0]
    server_id = requests.get('http://192.168.0.104:8080/get_server_id').json()[0]
    print(unique_id)
    logger.info("Уникальный id выдан успешно")
except requests.exceptions.ConnectionError:
    logger.error("Сервер не запущен")
    exit(-1)

client = mqtt_client.Client(client_id=unique_id)

min_brightness = None
max_brightness = None
threshold_brightness = None
prev_value = None

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    connection.write(cmd.encode())
    if response_len > 0:
        return connection.read(response_len).decode()
    return ""

def on_message(client, userdata, message):
    global min_brightness, max_brightness, threshold_brightness, prev_value

    topic = message.topic
    data = message.payload.decode("utf-8")
    logger.info(f"Принято сообщение на {topic}: {data}")

    if topic == f"lab/{server_id}/photo/min":
        min_brightness = int(data)
    elif topic == f"lab/{server_id}/photo/max":
        max_brightness = int(data)

    if min_brightness is not None and max_brightness is not None:
        threshold_brightness = (min_brightness + max_brightness) / 2

    if topic == f"lab/{server_id}/photo/stream":
        try:
            current_value = int(data)

            if threshold_brightness is not None:
                if current_value > threshold_brightness:
                    send_command('u', responses['u'], connection_led)
                else:
                    send_command('d', responses['d'], connection_led)
            prev_value = current_value
        except ValueError:
            logger.error(f"Неверный формат данных: {data}")

client.on_message = on_message

print("Подключение к брокеру", broker)
client.connect(broker)
client.loop_start()

client.subscribe(f"lab/{server_id}/photo/stream")
client.subscribe(f"lab/{server_id}/photo/min")
client.subscribe(f"lab/{server_id}/photo/max")

logger.info("Подписка на топики потока и минимальных/максимальных значений успешно выполнена")

try:
    while True:
        time.sleep(1)
except KeyboardInterrupt:
    logger.info("Остановка подписчика")

client.disconnect()
client.loop_stop()
