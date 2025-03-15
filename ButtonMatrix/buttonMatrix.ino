#define N 50
#define NROWS 3
#define NCOLS 3

const uint8_t row_pins[NROWS] = {2, 3, 4};
const uint8_t col_pins[NCOLS] = {5, 6, 7};

volatile uint8_t current_row = 0;
bool btn_state[NROWS * NCOLS] = {false};
bool btn_prev_state[NROWS * NCOLS] = {false};
unsigned long press_time[NROWS * NCOLS] = {0};
unsigned long press_start[NROWS * NCOLS] = {0};

void setup() {
    cli();
    TCCR1A = 0;
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = (F_CPU / 1024 / (1000 / N)) - 1;
    TIMSK1 |= (1 << OCIE1A);
    sei();
    
    DDRD |= 0b00011100;
    DDRD &= ~0b11100000;
    PORTD |= 0b11100000;
    Serial.begin(9600);
}

ISR(TIMER1_COMPA_vect) {
    current_row = (current_row + 1) % NROWS;
}

void loop() {
    update_button_state();
    delay(5);
}

void update_button_state() {
    PORTD |= 0b00011100;  
    PORTD &= ~(1 << (row_pins[current_row]));  
    
    uint8_t col_states = PIND & 0b11100000;
    col_states = ~col_states >> 5;
    
    bool changed = false;
    for (uint8_t icol = 0; icol < NCOLS; icol++) {
        uint8_t btn_index = current_row * NCOLS + icol;
        bool new_state = col_states & (1 << icol);
        if (new_state != btn_prev_state[btn_index]) {
            btn_prev_state[btn_index] = new_state;
            changed = true;
            if (new_state) {
                press_start[btn_index] = millis();
            } else {
                press_time[btn_index] = millis() - press_start[btn_index];
                Serial.print("duration ");
                Serial.print(btn_index + 1);
                Serial.print(" button - ");
                Serial.print(press_time[btn_index]);
                Serial.print(" ms, from ");
                Serial.println(press_start[btn_index]);
            }
        }
        btn_state[btn_index] = new_state;
    }
    
    if (changed) {
        Serial.print("Pressed button ");
        bool first = true;
        for (uint8_t i = 0; i < NROWS * NCOLS; i++) {
            if (btn_state[i]) {
                if (!first) Serial.print(", ");
                Serial.print(i + 1);
                first = false;
            }
        }
        Serial.println();
    }
}
