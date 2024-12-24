#define SHIFT_CLOCK 2
#define LATCH_CLOCK 3
#define DATA_INPUT 4
#define DELAY_TIME 1500
#define DELAY_AFTER_SERIAL 6000

int index = 0;
bool fl = false;
unsigned long int previousMillis = 0;
byte nums[10] = {
  0b00010001,  // 0
  0b11011101, // 1
  0b00101001, // 2
  0b10001001, // 3
  0b11000101, // 4
  0b10000011, // 5
  0b00000010, // 6
  0b11011001, // 7
  0b00000001, // 8
  0b10000000 // 9
};

unsigned long delayTime = DELAY_TIME;

void setup() {
  Serial.begin(9600);

  DDRD |= (1 << SHIFT_CLOCK) | (1 << LATCH_CLOCK) | (1 << DATA_INPUT);

  PORTD &= ~((1 << SHIFT_CLOCK) | (1 << LATCH_CLOCK) | (1 << DATA_INPUT));
}

void shiftOutData(byte value1, byte value2) {
  for (int i = 0; i < 8; i++) {
    if (value2 & (1 << i)) {
      PORTD |= (1 << DATA_INPUT);
    } else {
      PORTD &= ~(1 << DATA_INPUT);
    }

    PORTD |= (1 << SHIFT_CLOCK);
    delay(3);
    PORTD &= ~(1 << SHIFT_CLOCK);
  }

  for (int i = 0; i < 8; i++) {
    if (value1 & (1 << i)) {
      PORTD |= (1 << DATA_INPUT);
    } else {
      PORTD &= ~(1 << DATA_INPUT);
    }

    // Импульс сдвига на SHIFT_CLOCK
    PORTD |= (1 << SHIFT_CLOCK);
    delay(3);
    PORTD &= ~(1 << SHIFT_CLOCK);
  }

  // Импульс фиксации на LATCH_CLOCK
  PORTD |= (1 << LATCH_CLOCK);
  delay(6);
  PORTD &= ~(1 << LATCH_CLOCK);
}

void loop() {
  unsigned long int currentMillis = millis();

  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    int number = input.toInt();

    if (number >= 0 && number <= 99) {
      index = number;
      fl = true;
      delayTime = DELAY_AFTER_SERIAL;
      previousMillis = currentMillis;
      shiftOutData(nums[index / 10], nums[index % 10]); // Отправляем данные в оба регистра
    }
  }

  if (currentMillis - previousMillis >= delayTime) {
    previousMillis = currentMillis;

    if (!fl) { 
      index = (index + 1) % 100;
      shiftOutData(nums[index / 10], nums[index % 10]); // Отправляем данные в оба регистра
    }

    fl = false;
    delayTime = DELAY_TIME;
  }
}
