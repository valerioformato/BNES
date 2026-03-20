//
// Created by Valerio Formato on 20-Mar-26.
//

#ifndef BNES_JOYPAD_H
#define BNES_JOYPAD_H

#include "HW/Bus.h"

#include <bitset>
#include <cstdint>

namespace BNES::HW {
class Joypad {
public:
  enum class Button {
    BUTTON_A = 0,
    BUTTON_B,
    SELECT,
    START,
    UP,
    DOWN,
    LEFT,
    RIGHT,
  };

  Joypad() = delete;
  explicit Joypad(Bus &bus, unsigned int joy_no) : m_bus{&bus} { m_bus->Attach(this, joy_no++); };

  void SetButtonStatus(Button button, bool status);

  void Write(uint8_t value);
  uint8_t Read();

private:
  non_owning_ptr<Bus *> m_bus;

  bool m_strobe{false};
  uint8_t m_button_index{0};
  std::bitset<8> m_button_status;
};
} // namespace BNES::HW

#endif // BNES_JOYPAD_H
