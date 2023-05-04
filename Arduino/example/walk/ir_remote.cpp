#include "ir_remote.h"

#include <IRremote.h>
#include <IRremoteInt.h>

#include "hw_config.h"

// IR Remote Control key codes.
// M-07245 OE13KRIR: IR Remote Control 13-Key
#define IR_KEY_UP 0x5fa0ef10
#define IR_KEY_DOWN 0xff00ef10
#define IR_KEY_LEFT 0xef10ef10
#define IR_KEY_RIGHT 0x7f80ef10
#define IR_KEY_RIGHT_DOWN 0x7e81ef10
#define IR_KEY_RIGHT_UP 0xde21ef10
#define IR_KEY_LEFT_UP 0x4eb1ef10
#define IR_KEY_LEFT_DOWN 0xee11ef10

#define IR_KEY_CENTER 0xdf20ef10
#define IR_KEY_A 0x07f8ef10
#define IR_KEY_B 0x8778ef10
#define IR_KEY_C 0xa758ef10
#define IR_KEY_POWER 0x27d8ef10

#if BOARD_VERSION == 1
const int kRecvPin = 4;
#elif BOARD_VERSION == 2
const int kRecvPin = 2;
#else
#error "BOARD_VERSION not defined"
#endif

void IrRemote::Init() {
  IrReceiver.begin(kRecvPin);
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

  IRRawDataType key_code = prev_key_code;

  if (IrReceiver.decode()) {  // Grab an IR code
    IRRawDataType ir_data = IrReceiver.decodedIRData.decodedRawData;
    bool repeat =
        (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) != 0;
    result.carrier_detected = true;
    IrReceiver.resume();
    key_code = repeat ? prev_key_code : ir_data;
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
