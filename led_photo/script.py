import serial
import time
responses = {'d': 7,  # "led_off" 
             'u': 6,  # "led_on" 
             'p': 3}  # 0 -> 1023 zero fill to leftside
port_led = "/dev/ttyUSB0" # "COM4" or similar for windows
port_photo = "/dev/ttyUSB1" # "COM4" or similar for windows
connection_led = serial.Serial(port_led, baudrate=9600, timeout=0.1) # baudrate=9600
connection_photo = serial.Serial(port_photo, baudrate=9600, timeout=0.1) # baudrate=9600

MAX_TRIES = 10

def send_command(cmd: str, response_len: int, connection: serial.Serial) -> str:
    str_resp: str = ""
    connection.write(cmd.encode())
    
    if response_len > 0:
        #print(connection.in_waiting)
        #current_try = 0
        #while connection.in_waiting != response_len:
        #    current_try += 1
        #    if current_try > MAX_TRIES:
        #        break
        #    continue
        #print("After while", connection.in_waiting)
        resp = connection.read(response_len)
        str_resp = resp.decode()
    return str_resp

while True:
    photo_val_resp: str = send_command('p', responses['p'], connection_photo)
    try:
        photo_val = int(photo_val_resp)
        print(photo_val)
        if photo_val > 50:
            resp = send_command('u', responses['u'], connection_led)
        else:
            resp = send_command('d', responses['d'], connection_led)
    except Exception as e:
        print(e)
        continue