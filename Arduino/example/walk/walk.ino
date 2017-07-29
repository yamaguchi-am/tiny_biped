/**
 * Walk the robot by IR remote commands.
 * 
 * Remote control with M-07245 OE13KRIR: IR Remote Control 13-Key.
 *
 * == Key assignments ==
 * Up / Down : Walk forward / backward.
 * Left / Right : Turn left / right.
 * A : Go back to the home position.
 * B : Stand on the outer edges of the feet.
 *
 * Install Arduino-IRremote library before compiling this sketch.
 * https://github.com/z3t0/Arduino-IRremote
 */

#include <EEPROM.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <Servo.h>
#include <math.h>
#include "ir_remote.h"
#include "walk_controller.h"

const int kNumServos = 6;
const int kServoPinMap[kNumServos] = {
  3, 6, 9, 10, 11, 12
};

// The center position of servo commands, in microseconds.
const int kCenter = 1500;

Servo* myservos_;
IrRemote ir_remote;

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

void Prompt() {
  Serial.print("CH ");
  Serial.print(current_channel);
  Serial.print(" trim=");
  Serial.print(trim[current_channel]);
  Serial.println(" > ");
}

void Help() {
  Serial.println("j -- next servo channel");
  Serial.println("k -- previous servo channel");
  Serial.println("h -- decrease trim value");
  Serial.println("l -- increase trim value");
  Serial.println("     Trim values are reflected instantly, "
                  "but not persisted until saving it.");
  Serial.println("W -- save trim data to EEPROM");
  Serial.println("R -- load trim data from EEPROM");
  Serial.println("! -- toggle swing mode");
}

WalkController walk_controller;
float last_time = 0;

void setup() {
  Serial.begin(9600);
  ir_remote.Init();
  myservos_ = new Servo[kNumServos];
  for (int i = 0; i < kNumServos; i++) {
    myservos_[i].attach(kServoPinMap[i]);
  }
  LoadTrim();
  Serial.println("Enter '?' for help.");
  Prompt();
}

enum Mode {
  M_WALK, POSE_A, POSE_B, POSE_C  
} mode = M_WALK;

void loop() {
  ControllerInput input = ir_remote.Fetch();
  Command command;
  command.forward = input.StickY();
  command.turn = input.StickX();

  if (command.forward || command.turn) {
    mode = M_WALK;
  }
  if (input.Button(0)) {
    mode = POSE_A;
    walk_controller.Init();
  }
  if (input.Button(1)) {
    mode = POSE_B;
    walk_controller.Init();
  }
  float t = millis() / 1000.0;
  float diff = t - last_time;
  last_time = t;
  if (mode == M_WALK) {
    float angle[4];
    walk_controller.Update(diff, command);
    walk_controller.GetPose(angle);
    for (int i = 0; i < 4; i++) {
      OutputServoWithTrim(i, angle[i]);
    }
  } else if (mode == POSE_A) {
    OutputServoAllCenter();
  } else if (mode == POSE_B) {
    OutputServoWithTrim(0, 0);
    OutputServoWithTrim(1, 700);
    OutputServoWithTrim(2, 0);
    OutputServoWithTrim(3, -700);
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
    }
    Prompt();
  }

  delay(15);
}

