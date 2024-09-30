#define LED_PIN_RED 13
#define LED_PIN_YELLOW 12
#define LED_PIN_GREEN 11

#define GREEN_DURATION 10000
#define BLINKING_GREEN_DURATION 2000
#define YELLOW_DURATION 1000
#define RED_DURATION 7000
#define SWITCHING_YELLOW_DELAY 3000
#define MANUAL_TIMEOUT 60000

long int cycle_start = 0;
long int start_blinking_green = 0;
long int manual_mode_start = 0;
long int yellow_after_blinking_start = 0;

bool manual_mode = false;
bool requested_green = false;
bool requested_red = false;
bool manual_active = false;

bool is_red_on = false;
bool is_green_on = false;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN_RED, OUTPUT);
  pinMode(LED_PIN_YELLOW, OUTPUT);
  pinMode(LED_PIN_GREEN, OUTPUT);
  cycle_start = millis();
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    handle_manual_command(command);
  }

  if (manual_mode) {
    if (millis() - manual_mode_start > MANUAL_TIMEOUT) {
      manual_mode = false;
      manual_active = false;
      cycle_start = millis();
      turn_off_all_leds();  
    } else {
      manual_mode_operation();
    }
  } else {
    switch_traffic_light();
  }
}

void handle_manual_command(char command) {
  if (command == 'M') { 
    manual_mode = true;
    manual_mode_start = millis();
    turn_off_all_leds();
  } else if (command == 'G') {
    if (manual_mode && !manual_active) {
      if (is_green_on) {
        turn_off_green();
        is_green_on = false;
        manual_active = false;
      } else {
        requested_green = true;
        manual_active = true;
        turn_off_red();
        is_red_on = false;
        start_yellow_for_manual();
      }
    }
  } else if (command == 'R') {
    if (manual_mode && !manual_active) {
      if (is_red_on) {
        turn_off_red();
        is_red_on = false;
        manual_active = false;
      } else {
        requested_red = true;
        manual_active = true;
        turn_off_green();
        is_green_on = false;
        start_yellow_for_manual();
      }
    }
  }
}

void start_yellow_for_manual() {
  cycle_start = millis();
  turn_on_yellow();
}

void manual_mode_operation() {
  if (requested_green) {
    if (millis() >= cycle_start + SWITCHING_YELLOW_DELAY) {
      turn_off_yellow();
      turn_on_green();
      requested_green = false;
      manual_active = false;
    }
  } else if (requested_red) {
    if (millis() >= cycle_start + SWITCHING_YELLOW_DELAY) {
      turn_off_yellow();
      turn_on_red();
      requested_red = false;
      manual_active = false;
    }
  }
}

void switch_traffic_light() {
  if (millis() >= RED_DURATION + YELLOW_DURATION + GREEN_DURATION + BLINKING_GREEN_DURATION + cycle_start + YELLOW_DURATION) {
    cycle_start = millis();
    turn_off_green();
    turn_off_yellow();
  }
  else if (millis() >= RED_DURATION + YELLOW_DURATION + GREEN_DURATION + BLINKING_GREEN_DURATION + cycle_start) {
    if (yellow_after_blinking_start == 0) {
      yellow_after_blinking_start = millis();
    }
    if (millis() - yellow_after_blinking_start <= YELLOW_DURATION) {
      turn_on_yellow();
    } else {
      turn_off_yellow();
      yellow_after_blinking_start = 0;
    }
  }
  else if (millis() >= RED_DURATION + YELLOW_DURATION + GREEN_DURATION + cycle_start) {
    switching_green();
  }
  else if (millis() >= RED_DURATION + YELLOW_DURATION + cycle_start) {
    turn_on_green();
    turn_off_yellow();
  }
  else if (millis() >= RED_DURATION + cycle_start) {
    turn_on_yellow();
    turn_off_red();
  }
  else if (millis() >= cycle_start) {
    turn_on_red();
  }
}

void turn_on_red() {
  digitalWrite(LED_PIN_RED, HIGH);
  is_red_on = true;
}

void turn_on_yellow() {
  digitalWrite(LED_PIN_YELLOW, HIGH);
}

void turn_on_green() {
  digitalWrite(LED_PIN_GREEN, HIGH);
  is_green_on = true;
}

void turn_off_green() {
  digitalWrite(LED_PIN_GREEN, LOW);
  is_green_on = false;
}

void turn_off_yellow() {
  digitalWrite(LED_PIN_YELLOW, LOW);
}

void turn_off_red() {
  digitalWrite(LED_PIN_RED, LOW);
  is_red_on = false;
}

void switching_green() {
  if (start_blinking_green == 0) {
    start_blinking_green = millis();
  } else if (millis() - start_blinking_green <= BLINKING_GREEN_DURATION) {
    if ((millis() / 500) % 2 == 0) {
      turn_off_green();
    } else {
      turn_on_green();
    }
  } else {
    turn_off_green();
    start_blinking_green = 0;
  }
}

void turn_off_all_leds() {
  turn_off_red();
  turn_off_yellow();
  turn_off_green();
}