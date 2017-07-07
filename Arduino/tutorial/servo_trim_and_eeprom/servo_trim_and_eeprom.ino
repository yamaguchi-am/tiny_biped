/**
 * EEPROM and trim (center position adjustment) of servos.
 * 
 * Sets up the 6 servo output pins to output PWM signal.
 * Shows a console to interactively adjust servo trim.
 * 
 * j -- next servo channel
 * k -- previous servo channel
 * h -- decrease trim value
 * l -- increase trim value
 *      Trim values are reflected instantly, but not persisted until saving it.
 * W -- save trim data to EEPROM
 * R -- load trim data from EEPROM
 * ! -- toggle swing mode
 *      See 2 servos swing around the trimmed center position.
 *
 * It is recommended to use serial terminal software other than the serial
 * monitor in the Arduino IDE because you can execute commands without hitting
 * [enter] key.
 * 
 * Note that trim range is limited between -128 and 127[us] by this exapmle.
 * (When you need to adjust above this range, you can probably adjust it by
 * reassembling the connection between the servo horn and the servo.)
 */

#include <math.h>
#include <EEPROM.h>
#include <Servo.h>

const int kNumServos = 6;
const int kServoPinMap[kNumServos] = {
  3, 6, 9, 10, 11, 12
};

// The center position of servo commands, in microseconds.
const int kCenter = 1500;

// The amplitude of the swing motion in the test mode.
const float kAmplitudeMicros = 200;
const float kFrequency = 0.5;

Servo* myservos_;

int trim[6];

// In this example, we store offset value as a single 8bit signed integer.
int DecodeTrim(char data) {
  return static_cast<int>(data);
}

char EncodeTrim(int data) {
  return static_cast<char>(data);
}

void LoadTrim() {
  Serial.print("Loading trim data:");
  for (int i = 0; i < kNumServos; i++) {
    char data = EEPROM.read(i);
    trim[i] = DecodeTrim(data);
    Serial.print(trim[i]);
    Serial.print(" ");
  }
  Serial.println();
}

void SaveTrim() {
  Serial.print("Saving trim data...");
  for (int i = 0; i < kNumServos; i++) {
    char data = EncodeTrim(trim[i]);
    EEPROM.write(i, data);
  }
  Serial.println("done.");
}

void OutputServoWithTrim(int id, int micros_relative_to_center) {
  myservos_[id].writeMicroseconds(kCenter + trim[id] + micros_relative_to_center);
}

void OutputServoAllCenter() {
  for (int i = 0; i < kNumServos; i++) {
    OutputServoWithTrim(i, 0);
  }
}

int current_channel = 0;
bool enable_swing = false;

void Prompt() {
  Serial.print("CH ");
  Serial.print(current_channel);
  Serial.print(" trim=");
  Serial.print(trim[current_channel]);
  Serial.println(" > ");
}

void setup() {
  Serial.begin(9600);
  myservos_ = new Servo[kNumServos];
  for (int i = 0; i < kNumServos; i++) {
    myservos_[i].attach(kServoPinMap[i]);
  }
  LoadTrim();
  Serial.println("Enter '?' for help.");
  Prompt();
}

void Help() {
  Serial.println("j -- next servo channel");
  Serial.println("k -- previous servo channel");
  Serial.println("h -- decrease trim value");
  Serial.println("l -- increase trim value");
  Serial.println("     Trim values are reflected instantly, but not persisted until saving it.");
  Serial.println("W -- save trim data to EEPROM");
  Serial.println("R -- load trim data from EEPROM");
  Serial.println("! -- toggle swing mode");
}
void loop() {
  if (enable_swing) {
    float t = millis() / 1000.0;
    OutputServoWithTrim(1, kAmplitudeMicros * sin(2 * M_PI * t * kFrequency));
    OutputServoWithTrim(3, kAmplitudeMicros * sin(2 * M_PI * t * kFrequency));
  } else {
    OutputServoAllCenter();
  }

  if (Serial.available()) {
    char c = Serial.read();
    switch(c) {
    case 'j':
      current_channel = (current_channel < kNumServos - 1) ?
                        current_channel + 1 : 0;
      break;
    case 'k':
      current_channel = (current_channel > 0) ?
                        current_channel - 1 : kNumServos - 1;
      break;
    case 'h':
      trim[current_channel] = max(-128, trim[current_channel] - 10);
      break;
    case 'l':
      trim[current_channel] = min(127, trim[current_channel] + 10);
      break;
    case 'R':
      LoadTrim();
      break;
    case 'W':
      SaveTrim();
      break;
    case '?':
      Help();
      break;
    case '!':
      enable_swing = !enable_swing;
    }
    Prompt();
  }
  delay(15);
}

