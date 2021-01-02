#include "remote_controller.h"

class IrRemote : public RemoteController {
 public:
  IrRemote() : no_carrier_count_(0) {};
  void Init();

  // Fetch current data and return buttons.
  ControllerInput Fetch();

 private:
  int no_carrier_count_;
  long prev_key_code;
};

