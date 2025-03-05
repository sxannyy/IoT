import re
import serial
import time
import numpy as np

class UFModel:
    def __init__(self):
        self.max_attempts = 10
        self.mcu = "/dev/ttyUSB0" 
        self.response = 5
        self.mcu_connection = serial.Serial(self.mcu, baudrate=9600, timeout=0.7)
        self.measures_dist = {}
        print("Establishing connection...")
        if self.__check_connection():
            print("Connection established")
        else:
            print("Connection failed")
            exit(-1)

    def __check_connection(self):
        cmd = "U"
        attempts = 0
        while attempts != self.max_attempts:
            bytes_written = self.mcu_connection.write(cmd.encode())
            if self.mcu_connection.in_waiting:
                self.mcu_connection.read(self.response)
                return True
            else:
                attempts += 1
                time.sleep(0.5)

        return False
    
    def send_command(self, cmd: str) -> str:
        str_resp: str = ""
        attempts = 0
        while attempts != self.max_attempts:
            bytes_written = self.mcu_connection.write(cmd.encode())
            # print(f"Writing {cmd}, wrote {bytes_written}")

            if self.mcu_connection.in_waiting:
                # print("Success Attempt num:", attempts, "in_waiting val:", self.mcu_connection.in_waiting)
                resp = self.mcu_connection.read(self.response)
                str_resp = resp.decode('utf').rstrip('\n')
                # print(str_resp)
                break
            else:
                # print("Failed Attempt num:", attempts, "in_waiting val:", self.mcu_connection.in_waiting)
                attempts += 1
                time.sleep(0.2)
        return str_resp

    def calc_model(self):
        ir_data = []
        us_data = []

        # Собираем данные с датчиков
        for _ in range(100):  # Собираем 100 измерений для построения модели
            resp_IR = 0
            resp_US = 0
            for _ in range(4):
                resp_IR = self.send_command("I")
                resp_US = self.send_command("U")
            print(resp_IR, '-', float(resp_US) * .0343 / 2)
            ir_data.append(float(resp_IR))
            us_data.append(float(resp_US) * .0343 / 2)
            time.sleep(1)  # Небольшая задержка между измерениями

        # Преобразуем списки в массивы numpy для удобства вычислений
        ir_data = np.array(ir_data)
        us_data = np.array(us_data)

        # Выполняем линейную регрессию: us_data = a * ir_data + b
        A = np.vstack([ir_data, np.ones(len(ir_data))]).T
        a, b = np.linalg.lstsq(A, us_data, rcond=None)[0]

        print(f"Коэффициенты линейной регрессии: a = {a}, b = {b}")

        # Возвращаем коэффициенты для использования в дальнейшем
        return a, b




