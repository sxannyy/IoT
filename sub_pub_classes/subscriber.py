import time
import paho.mqtt.client as mqtt_client
import requests
from loguru import logger
import serial

responses = {'d': 7,
             'u': 6,
             'p': 3}
port_led = "/dev/ttyUSB0"
connection_led = serial.Serial(port_led, baudrate=9600, timeout=0.1)

logger.add('logs/logs.log')

broker="broker.emqx.io"

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    str_resp: str = ""
    connection.write(cmd.encode())
    
    if response_len > 0:
        resp = connection.read(response_len)
        str_resp = resp.decode()
    return str_resp


def on_message(client, userdata, message):
    time.sleep(1)
    data = str(message.payload.decode("utf-8"))
    logger.info("Успешно принято сообщение - " + data)
    try:
        if data == 'on':
            resp = send_command('u', responses['u'], connection_led)
        else:
            resp = send_command('d', responses['d'], connection_led)
    except Exception as e:
        print(e)


try:
    unique_id = requests.get('http://172.20.10.3:8000/get_id').json()[0]
    print(unique_id)
    logger.info("Уникальный id выдан успешно")
except requests.exceptions.ConnectionError:
    logger.error("Сервер не запущен")
    exit(-1)

client = mqtt_client.Client(
   mqtt_client.CallbackAPIVersion.VERSION1, 
   unique_id
)
client.on_message=on_message

print("Connecting to broker",broker)
client.connect(broker) 
client.loop_start() 
print("Subcribing")
logger.info("Подписка прошла успешно")
client.subscribe(requests.get('http://172.20.10.3:8000/get_topic').json()[0])
time.sleep(500)
client.disconnect()
logger.info("Остановление подписки")
client.loop_stop()

