/**
 * IR remote receiver
 *
 * Sets up the IR receiver pin to read the IR receiver unit.
 * Outputs received command codes to serial terminal.
 * Toggles the LED on Arduino nano board by A/B buttons.
 *
 * Install Arduino-IRremote library before compiling this sketch.
 * https://github.com/z3t0/Arduino-IRremote
 *
 * See the documents and example programs of the library for more details.
 */

#include <IRremote.h>
#include <IRremoteInt.h>

const int kRecvPin = 2;
// If you use the v1 board (green color), replace above line with this:
// const int kRecvPin = 4;
const int kLedPin = 13;

// Key codes.
// M-07245 OE13KRIR: IR Remote Control 13-Key
const unsigned long kIrKeyA = 0x8f71fe0;
const unsigned long kIrKeyB = 0x8f71ee1;

IRrecv irrecv(kRecvPin);

void setup() {
  Serial.begin(9600);
  irrecv.enableIRIn();
}

void loop() {
  decode_results ir_data;
  if (irrecv.decode(&ir_data)) {
    Serial.println(ir_data.value, HEX);
    irrecv.resume();
    if (ir_data.value == kIrKeyA) {
      digitalWrite(kLedPin, HIGH);
    } else if (ir_data.value == kIrKeyB) {
      digitalWrite(kLedPin, LOW);
    }
  }
}
