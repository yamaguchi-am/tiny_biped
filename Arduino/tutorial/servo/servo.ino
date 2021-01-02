/**
 * Output servo signals.
 *
 * Sets up the 6 servo output pins to output PWM signal.
 * Swings the servo connected to #0.
 */

#include <Servo.h>
#include <math.h>

// The board has 6 servo ports connected to the 6 digital output pins.
const int kNumServos = 6;
const int kServoPinMap[kNumServos] = {3, 6, 9, 10, 11, 12};

// The center position for servo commands.
const int kCenter = 1500;

// This example specifies angles by microseconds because it doesn't care
// physical angles. If you want to specify the position in degrees or radians,
// you'd need to do conversion by yourself after measuring the micros-to-angle
// response.
const float kAmplitudeMicros = 200;
const float kFrequency = 0.5;

Servo* myservos_;

void setup() {
  Serial.begin(9600);
  myservos_ = new Servo[kNumServos];
  for (int i = 0; i < kNumServos; i++) {
    myservos_[i].attach(kServoPinMap[i]);
  }
  for (int i = 0; i < kNumServos; i++) {
    myservos_[i].writeMicroseconds(kCenter);
  }
}

void loop() {
  float t = millis() / 1000.0;
  myservos_[0].writeMicroseconds(kCenter + kAmplitudeMicros *
                                               sin(2 * M_PI * t * kFrequency));
  // Avoid updating pulse width too frequently.
  // It may not necessarily have bad effects, but will not take effect
  // as the PWM signal is only produced at 15~20ms interval.
  delay(15);
}
