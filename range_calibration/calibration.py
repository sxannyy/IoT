from functions import UFModel
import serial

model = UFModel() 
a, b = model.calc_model()
print()
print()
print(f"Полученные коэффициенты:{a}, {b}")
for i in range(3):
    print(f"Измерение звуком №{i + 1} - {int(model.send_command('U')) * .0343 / 2}")
print()
for i in range(3):
    print(f"Измерение инфракрасным датчиком №{i + 1} - {a * float(model.send_command('I')) + b}")

