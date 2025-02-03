#define BUTTON_PIN 2
#define BOUNCE_TIME 60
#define PRESSED HIGH
#define TIMER_INTERVAL 5
#define DEBOUNCE_COUNT 10 // Количество прерываний для устранения дребезга

volatile long int press_time = 0;
volatile long int hold_time = 0;
volatile bool pressed_candidate = false;
volatile int press_count = 0;
volatile int debounce_counter = 0;
bool button_pressed = false;
bool report_to_user = false;

void setup()
{
  pinMode(BUTTON_PIN, INPUT);
  Serial.begin(9600);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), process_button_click, RISING);
  setupTimerInterrupt();
}

void loop()
{
  if (report_to_user) {
    if (digitalRead(BUTTON_PIN) == LOW) {
      hold_time = millis() - press_time;
      press_count++;
      Serial.print("New press, time of pressing - ");
      Serial.print(hold_time);
      Serial.print(" ms. Count of presses - ");
      Serial.println(press_count);
      report_to_user = false;
    }
  }
}

void process_button_click() {
  if (!pressed_candidate) {
    press_time = millis();
    pressed_candidate = true;
    debounce_counter = 0;
  }
}

void setupTimerInterrupt() {
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  
  // 16 МГц / 64 / 1000 * 5 мс
  OCR1A = 1250;
  
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS11) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  
  interrupts();
}

ISR(TIMER1_COMPA_vect) {
  if (pressed_candidate) {
    debounce_counter++;
    
    bool button_state = (PINB & (1 << BUTTON_PIN)) != 0;   
    if (debounce_counter > DEBOUNCE_COUNT) {
      if (button_state == PRESSED) {
        report_to_user = true;
      }
      pressed_candidate = false;
    }
  }
}
