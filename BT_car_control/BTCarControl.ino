#include <SoftwareSerial.h>

enum Mode : uint8_t { IDLE, WHEELS, DIRECTION, SPEED, ROTATION };
Mode mode = IDLE;

enum Key : char {
  KEY_TRIANGLE = 'T',
  KEY_CIRCLE   = 'C',
  KEY_CROSS    = 'X',
  KEY_SQUARE   = 'S',
  KEY_START    = 'A',

  KEY_UP       = 'F',
  KEY_DOWN     = 'B',
  KEY_RIGHT    = 'R',
  KEY_LEFT     = 'L',
  KEY_RELEASE  = '0'
};

const uint8_t LEFT_PWM  = 6;
const uint8_t RIGHT_PWM = 5;

uint8_t LEFT_DIR  = 7;
uint8_t RIGHT_DIR = 4;

/* ---------- направления (можно инвертировать в режиме DIRECTION) ---------- */
bool LEFT_FWD  = HIGH;
bool LEFT_REV  = LOW;
bool RIGHT_FWD = HIGH;
bool RIGHT_REV = LOW;

/* ---------- рабочие параметры ---------- */
int    baseSpeed     = 150;      // базовая ШИМ-скорость 0-255
double speedCoef     = 1.00;     // баланс правого колеса
int    balanceOffset = baseSpeed;/* используется в режиме WHEELS */

uint32_t rotTime_ms  = 1000;     // начальное время разворота в режиме ROTATION
uint32_t rotStart    = 0;

/* ---------- Serial ---------- */
SoftwareSerial BT(10, 11);       // 10 → RX HC-05, 11 → TX HC-05

/* ---------- вспомогательные функции ---------- */
void drive(bool dirL, int spL, bool dirR, int spR) {
  analogWrite(LEFT_PWM,  constrain(spL,  0, 255));
  analogWrite(RIGHT_PWM, constrain(spR, 0, 255));
  digitalWrite(LEFT_DIR,  dirL);
  digitalWrite(RIGHT_DIR, dirR);
}

void stop() { drive(LOW, 0, LOW, 0); }

void goForward()  { drive(LEFT_FWD,  baseSpeed, RIGHT_FWD, int(baseSpeed * speedCoef)); }
void goBackward() { drive(LEFT_REV,  baseSpeed, RIGHT_REV,  int(baseSpeed * speedCoef)); }
void spinLeft()   { drive(LEFT_REV,  baseSpeed, RIGHT_FWD,  int(baseSpeed * speedCoef)); }
void spinRight()  { drive(LEFT_FWD,  baseSpeed, RIGHT_REV,  int(baseSpeed * speedCoef)); }

/* ---------- setup ---------- */
void setup() {
  Serial.begin(9600);
  BT.begin(9600);

  pinMode(LEFT_PWM,  OUTPUT);
  pinMode(RIGHT_PWM, OUTPUT);
  pinMode(LEFT_DIR,  OUTPUT);
  pinMode(RIGHT_DIR, OUTPUT);
  stop();
}

/* ---------- loop ---------- */
void loop() {

  /* завершение автоповорота */
  if (mode == ROTATION && millis() - rotStart >= rotTime_ms) stop();

  if (BT.available()) {
    Key k = (Key)BT.read();

    /* ---------------- переключение режимов ---------------- */
    if (k == KEY_TRIANGLE)  mode = WHEELS;
    if (k == KEY_CIRCLE)    mode = SPEED;
    if (k == KEY_CROSS)     mode = ROTATION;
    if (k == KEY_SQUARE)    mode = DIRECTION;
    if (k == KEY_START)     mode = IDLE;

    /* ---------------- режим баланса колёс ---------------- */
    if (mode == WHEELS) {
      drive(LEFT_FWD, balanceOffset, RIGHT_FWD, baseSpeed - balanceOffset);

      if (k == KEY_LEFT || k == KEY_RIGHT) {
        uint8_t tmp = LEFT_DIR; LEFT_DIR = RIGHT_DIR; RIGHT_DIR = tmp;
        balanceOffset = baseSpeed - balanceOffset;   // «зеркалим» смещение
      }
    }

    /* ---------------- режим калибровки скорости ----------- */
    else if (mode == SPEED) {
      if (k == KEY_UP)        goForward();
      else if (k == KEY_DOWN) goBackward();
      else if (k == KEY_RELEASE) stop();
      else if (k == KEY_RIGHT) speedCoef = min(speedCoef + 0.05, 1.50);
      else if (k == KEY_LEFT)  speedCoef = max(speedCoef - 0.05, 0.50);
    }

    /* ---------------- инверсия направлений ---------------- */
    else if (mode == DIRECTION) {
      goForward();  // «показываем» текущее направление
      if (k == KEY_UP || k == KEY_DOWN)   { LEFT_FWD ^= 1;  LEFT_REV ^= 1;  }
      if (k == KEY_LEFT || k == KEY_RIGHT){ RIGHT_FWD ^= 1; RIGHT_REV ^= 1; }
    }

    /* ---------------- настройка времени разворота -------- */
    else if (mode == ROTATION) {
      if (k == KEY_UP)   rotTime_ms += 100;
      if (k == KEY_DOWN) rotTime_ms = (rotTime_ms > 100) ? rotTime_ms - 100 : 0;
      if (k == KEY_LEFT) { spinLeft();  rotStart = millis(); }
      if (k == KEY_RIGHT){ spinRight(); rotStart = millis(); }
    }

    /* ---------------- обычное управление ------------------ */
    else if (mode == IDLE) {
      if (k == KEY_UP)          goForward();
      else if (k == KEY_DOWN)   goBackward();
      else if (k == KEY_LEFT) { spinLeft();  rotStart = millis(); }
      else if (k == KEY_RIGHT){ spinRight(); rotStart = millis(); }
      else if (k == KEY_RELEASE) stop();
    }
  }

  /* сквозной мост ПК ↔ Bluetooth (по желанию) */
  if (Serial.available()) BT.write(Serial.read());
}
