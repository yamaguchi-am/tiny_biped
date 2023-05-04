#include "remote_controller.h"

// We cannot include the header here because it defines an instance.
// #include "IRRemote.hpp"
// Instead define the equivalent type used in IRRemote.
#include <stdint.h>
#if (__INT_WIDTH__ < 32)
typedef uint32_t IRRemote_IRRawDataType;
#else
typedef uint64_t IRRemote_IRRawDataType;
#endif

class IrRemote : public RemoteController {
 public:
  IrRemote() : no_carrier_count_(0){};
  void Init();

  // Fetch current data and return buttons.
  ControllerInput Fetch();

 private:
  int no_carrier_count_;
  // IRRawDataType prev_key_code;
  IRRemote_IRRawDataType prev_key_code;
};
