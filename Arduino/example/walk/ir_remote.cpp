#include "ir_remote.h"

#include <IRremote.h>
#include <IRremoteInt.h>

#include "hw_config.h"

// IR Remote Control key codes.
// M-07245 OE13KRIR: IR Remote Control 13-Key
#define IR_CONTINUE 0xffffffff
#define IR_KEY_UP 0x8f705fa
#define IR_KEY_DOWN 0x8f700ff
#define IR_KEY_LEFT 0x8f708f7
#define IR_KEY_RIGHT 0x8f701fe
#define IR_KEY_RIGHT_DOWN 0x8f7817e
#define IR_KEY_RIGHT_UP 0x8f7847b
#define IR_KEY_LEFT_UP 0x8f78d72
#define IR_KEY_LEFT_DOWN 0x8f78877

#define IR_KEY_CENTER 0x8f704fb
#define IR_KEY_A 0x8f71fe0
#define IR_KEY_B 0x8f71ee1
#define IR_KEY_C 0x8f71ae5
#define IR_KEY_POWER 0x8f71be4

#if BOARD_VERSION == 1
const int recvPin = 4;
#elif BOARD_VERSION == 2
const int recvPin = 2;
#else
#error "BOARD_VERSION not defined"
#endif

IRrecv irrecv(recvPin);

void IrRemote::Init() {
  irrecv.enableIRIn();  // Start the receiver
  prev_key_code = 0;
}

bool DecodeEightDirections(long keycode, float* out_x, float* out_y) {
  *out_x = 0;
  *out_y = 0;
  switch (keycode) {
    case IR_KEY_RIGHT:
      *out_x = 1;
      break;
    case IR_KEY_LEFT:
      *out_x = -1;
      break;
    case IR_KEY_UP:
      *out_y = 1;
      break;
    case IR_KEY_DOWN:
      *out_y = -1;
      break;
  }
}

ControllerInput IrRemote::Fetch() {
  ControllerInput result;
  decode_results ir_data;

  long key_code = prev_key_code;

  if (irrecv.decode(&ir_data)) {  // Grab an IR code
    result.carrier_detected = true;
    irrecv.resume();
    key_code = (ir_data.value == IR_CONTINUE) ? prev_key_code : ir_data.value;
    prev_key_code = key_code;
    no_carrier_count_ = 0;
  } else {
    result.carrier_detected = false;
    if (no_carrier_count_ >= 20) {
      key_code = 0;
      prev_key_code = key_code;
    } else {
      no_carrier_count_++;
    }
  }
  DecodeEightDirections(key_code, &result.stick_x, &result.stick_y);
  switch (key_code) {
    case IR_KEY_A:
      result.buttons[0] = true;
      break;
    case IR_KEY_B:
      result.buttons[1] = true;
      break;
    case IR_KEY_C:
      result.buttons[2] = true;
      break;
    case IR_KEY_CENTER:
      result.buttons[3] = true;
      break;
    case IR_KEY_POWER:
      result.buttons[4] = true;
      break;
  }
  return result;
}
