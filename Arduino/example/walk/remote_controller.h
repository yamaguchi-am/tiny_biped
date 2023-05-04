#ifndef REMOTE_CONTROLLER_H_
#define REMOTE_CONTROLLER_H_

struct ControllerInput {
  ControllerInput() : stick_x(0), stick_y(0), carrier_detected(false) {
    for (int i = 0; i < 10; i++) {
      buttons[i] = false;
    }
  }
  float StickX() const { return stick_x; };
  float StickY() const { return stick_y; };
  bool Button(int id) const { return buttons[id]; };
  bool buttons[10];
  float stick_x;
  float stick_y;
  bool carrier_detected;
};

class RemoteController {
 public:
  RemoteController() {}
  virtual void Init() = 0;

  // Fetch current data and return buttons.
  virtual ControllerInput Fetch() = 0;
};

#endif  // REMOTE_CONTROLLER_H_
