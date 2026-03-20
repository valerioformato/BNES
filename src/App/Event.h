#ifndef BNES_APP_EVENT_H
#define BNES_APP_EVENT_H

#include "HW/Joypad.h"

#include <variant>

namespace BNES {
struct QuitEvent {};
struct StepEvent {};
struct ContinueEvent {};

struct JoypadEvent {
  bool status;
  unsigned int joy_no;
  HW::Joypad::Button button;
};

using Event = std::variant<std::monostate, QuitEvent, StepEvent, ContinueEvent, JoypadEvent>;
} // namespace BNES

#endif
