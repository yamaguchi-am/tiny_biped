/**
 * IR remote receiver
 *
 * Sets up the IR receiver pin to read the IR receiver unit.
 * Outputs received command codes to serial terminal.
 * Toggles the LED on Arduino nano board by A/B buttons.
 *
 * Install Arduino-IRremote library before compiling this sketch.
 * https://github.com/Arduino-IRremote/Arduino-IRremote
 *
 * See the documents and example programs of the library for more details.
 */

#include <IRremote.h>

const int kRecvPin = 2;
// If you use the v1 board (green color), replace above line with this:
// const int kRecvPin = 4;
const int kLedPin = 13;

// Key codes.
// M-07245 OE13KRIR: IR Remote Control 13-Key
const IRRawDataType kIrKeyA = 0x07f8ef10;
const IRRawDataType kIrKeyB = 0x8778ef10;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(kRecvPin);
}

void loop() {
  if (IrReceiver.decode()) {
    IRRawDataType ir_data = IrReceiver.decodedIRData.decodedRawData;
    if ((IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) != 0) {
      Serial.println("repeat");
    } else {
      Serial.println(ir_data, HEX);
    }
    IrReceiver.resume();
    if (ir_data == kIrKeyA) {
      digitalWrite(kLedPin, HIGH);
    } else if (ir_data == kIrKeyB) {
      digitalWrite(kLedPin, LOW);
    }
  }
}
