uint32_t next_sample_time;
uint32_t next_write_time;
uint32_t time_unit = 100000;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.begin(115200);
  Serial.println("Initialization complete");

  pinMode(3, OUTPUT); // GND connection
  digitalWrite(3, LOW);

  pinMode(2, INPUT); // Signal input

  pinMode(7, OUTPUT); // Data pin
  pinMode(6, OUTPUT); // Clock pin
  pinMode(5, OUTPUT); // Output Enable

  show_digit(0);

  attachInterrupt(digitalPinToInterrupt(2), synchronize, CHANGE);
}

static void show_digit(int number) {
  const char lookup[] = {
    0b11101101, // 0
    0b10001000, // 1
    0b01101110, // 2
    0b11001110, // 3
    0b10001011, // 4
    0b11000111, // 5
    0b11110111, // 6
    0b10001100, // 7
    0b11101111, // 8
    0b11011111  // 9
  };

  digitalWrite(6, LOW);
  shiftOut(7, 5, LSBFIRST, lookup[number]);
  digitalWrite(6, HIGH);
}

typedef struct { uint8_t key; char character; uint8_t led_mask; } morse_entry_t;

morse_entry_t table1[] = {
  {0b0, 'E', 0b01100111},
  {0b1, 'T', 0b01100011},
  {0, 0, 0}
};

morse_entry_t table2[] = {
  {0b01, 'A', 0b10101111},
  {0b00, 'I', 0b00000001},
  {0b11, 'M', 0b10100100},
  {0b10, 'N', 0b10100010},
  {0, 0, 0}
};

morse_entry_t table3[] = {
  {0b100, 'D', 0b11101010},
  {0b110, 'G', 0b11001111},
  {0b101, 'K', 0b10101011},
  {0b111, 'O', 0b11100010},
  {0b010, 'R', 0b00100010},
  {0b000, 'S', 0b11000111},
  {0b001, 'U', 0b11101001},
  {0b011, 'W', 0b01001001},
  {0, 0, 0}
};

morse_entry_t table4[] = {
  {0b1000, 'B', 0b11100011},
  {0b1010, 'C', 0b01100101},
  {0b0010, 'F', 0b00100111},
  {0b0000, 'H', 0b10100011},
  {0b0111, 'J', 0b11101000},
  {0b0100, 'L', 0b01100001},
  {0b0110, 'P', 0b00101111},
  {0b1101, 'Q', 0b10001111},
  {0b0001, 'V', 0b11100000},
  {0b1001, 'X', 0b10111011},
  {0b1011, 'Y', 0b11001011},
  {0b1100, 'Z', 0b01101110},
  {0, 0, 0}
};

class MorseEncoder {
public:
  uint8_t current_sequence = 0;
  uint8_t remaining_bits = 0;
  uint8_t bit_buffer = 0;
  uint8_t bits_left = 0;

  bool is_valid(morse_entry_t table[], char letter) {
    for (int i = 0; table[i].character; i++)
      if (table[i].character == letter)
        return true;
    return false;
  }

  uint8_t find_code(morse_entry_t table[], char letter) {
    for (int i = 0; table[i].character; i++)
      if (table[i].character == letter)
        return table[i].key;
    return 0;
  }

  bool queue_character(char c) {
    if (bits_left) return false;

    if (c == ' ') {
      bit_buffer = 0b11;
      bits_left = 2;
      return true;
    } else if (is_valid(table1, c)) {
      current_sequence = find_code(table1, c);
      remaining_bits = 1;
    } else if (is_valid(table2, c)) {
      current_sequence = find_code(table2, c);
      remaining_bits = 2;
    } else if (is_valid(table3, c)) {
      current_sequence = find_code(table3, c);
      remaining_bits = 3;
    } else if (is_valid(table4, c)) {
      current_sequence = find_code(table4, c);
      remaining_bits = 4;
    } else {
      Serial.println("MorseEncoder: Unsupported character");
      return false;
    }

    update_buffer();
    return true;
  }

  void update_buffer() {
    if (remaining_bits == 0) return;

    remaining_bits--;
    uint8_t next_symbol = (current_sequence >> remaining_bits) & 1;

    if (next_symbol) {
      bit_buffer = 0b1000;
      bits_left = 4;
    } else {
      bit_buffer = 0b10;
      bits_left = 2;
    }

    if (remaining_bits == 0) {
      bit_buffer |= (0b11 << bits_left);
      bits_left += 2;
    }
  }

  bool get_signal_level() {
    if (bits_left == 0) return true;

    uint8_t bit = bit_buffer & 1;
    bit_buffer >>= 1;
    bits_left--;

    if (bits_left == 0) update_buffer();

    return bit;
  }
};

class MorseDecoder {
public:
  uint8_t accumulated_value = 0;
  uint8_t bit_count = 0;

  char decode(morse_entry_t table[], uint8_t key) {
    for (int i = 0; table[i].character; i++)
      if (table[i].key == key) {
        digitalWrite(6, LOW);
        shiftOut(7, 5, LSBFIRST, table[i].led_mask);
        digitalWrite(6, HIGH);
        return table[i].character;
      }
    return '?';
  }

  void add_dot() {
    accumulated_value = (accumulated_value << 1);
    bit_count++;
  }

  void add_dash() {
    accumulated_value = (accumulated_value << 1) + 1;
    bit_count++;
  }

  void finalize() {
    switch (bit_count) {
      case 1:
        Serial.print(decode(table1, accumulated_value));
        break;
      case 2:
        Serial.print(decode(table2, accumulated_value));
        break;
      case 3:
        Serial.print(decode(table3, accumulated_value));
        break;
      case 4:
        Serial.print(decode(table4, accumulated_value));
        break;
      default:
        Serial.println("MorseDecoder: Invalid length");
        break;
    }
    accumulated_value = 0;
    bit_count = 0;
  }
};

class SignalHandler {
public:
  enum ReceiverState {
    NO_SIGNAL,
    WAITING,
    SPACING,
    ACTIVE
  } state = NO_SIGNAL;

  MorseDecoder decoder;
  int time_counter = 0;

  void switch_to_waiting() { state = WAITING; time_counter = 0; }
  void switch_to_spacing() { state = SPACING; time_counter = 0; }
  void switch_to_active() { state = ACTIVE; time_counter = 0; }
  void switch_to_no_signal() { state = NO_SIGNAL; time_counter = 0; }

  void handle_signal(bool signal_present) {
    if (signal_present) {
      switch (state) {
        case WAITING:
          time_counter++;
          return;
        case SPACING:
          time_counter++;
          if (time_counter > 4) {
            Serial.println("[End of Character]");
            switch_to_waiting();
          } else if (time_counter == 4) {
            digitalWrite(6, LOW);
            shiftOut(7, 5, LSBFIRST, 0);
            digitalWrite(6, HIGH);
            Serial.print(" ");
          } else if (time_counter == 1) {
            decoder.finalize();
          }
          return;
        case ACTIVE:
          time_counter++;
          switch (time_counter) {
            case 1:
              decoder.add_dot();
              break;
            case 2:
            case 3:
              decoder.add_dash();
              break;
            default:
              Serial.print(" [Invalid Signal] ");
              break;
          }
          switch_to_spacing();
          return;
        case NO_SIGNAL:
          Serial.println("Signal Detected");
          Serial.print("Speed - ");
          Serial.println(analogRead(A5));
          switch_to_waiting();
          return;
      }
    } else {
      switch (state) {
        case WAITING:
          Serial.print("[Start Signal] ");
          switch_to_active();
          return;
        case SPACING:
          switch_to_active();
          return;
        case ACTIVE:
          time_counter++;
          if (time_counter > 4) {
            Serial.println("Connection Lost");
            switch_to_no_signal();
          }
          return;
        case NO_SIGNAL:
          time_counter++;
          return;
      }
    }
  }
};

MorseEncoder encoder;
SignalHandler handler;

void read_sample() {
  if (micros() >= next_sample_time) {
    handler.handle_signal(digitalRead(2));
    next_sample_time += time_unit;
  }
}

void write_signal() {
  if (micros() >= next_write_time) {
    digitalWrite(3, encoder.get_signal_level());
    next_write_time += time_unit;
  }
}

void loop()
{
  int potentiometer_value = analogRead(A5);
  time_unit = map(potentiometer_value, 0, 1023, 10000, 1000000);

  if (Serial.available()) {
    if (encoder.queue_character(Serial.peek()))
      Serial.read();
  }

  read_sample();
  write_signal();
}

void synchronize() {
  next_sample_time = micros() + time_unit / 2;
}
