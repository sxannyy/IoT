#define LED_PIN 13
#define SET_LED_ON 'u'
#define SET_LED_OFF 'd'
#define SET_LED_BLINK 'b'

#define STATE_IDLE 0
#define STATE_LED_ON 1
#define STATE_LED_OFF 2
#define STATE_LED_BLINK 3

#define BLINK_INTERVAL 5000

int state = 0;
long int blink_start = 0;
bool led_status = LOW;

void update_state() {
    if(Serial.available() > 0) {
      char cmd = Serial.read();
      if(cmd == SET_LED_ON) {
        state = STATE_LED_ON;
      } else if(cmd == SET_LED_OFF) {
        state = STATE_LED_OFF;
      } else if(cmd == SET_LED_BLINK) {
        state = STATE_LED_BLINK;
        blink_start = millis();
      } else {
        Serial.println("Unknown command");
      }
  }
}

void process_state() {
  if (state == STATE_LED_ON) {
    digitalWrite(LED_PIN, HIGH);
  } else if(state == STATE_LED_OFF) {
    digitalWrite(LED_PIN, LOW);
  } else if (state == STATE_LED_BLINK) {
    if (millis() <= blink_start + BLINK_INTERVAL and 
        led_status == LOW) {
      digitalWrite(LED_PIN, HIGH);
      led_status = HIGH;
    } 
    if (millis() >= blink_start + BLINK_INTERVAL and 
        millis() <= blink_start + 2 * BLINK_INTERVAL and
        led_status == HIGH) {
      digitalWrite(LED_PIN, LOW);
      led_status = LOW;
    } 
    if (millis() >= blink_start + 2 * BLINK_INTERVAL) {
      blink_start = millis();
    }
  }
  
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  update_state();
  process_state();
}