//
// Created by Valerio Formato on 20-Mar-26.
//

#include "Joypad.h"

namespace BNES::HW {

void Joypad::SetButtonStatus(Button button, bool status) { m_button_status[std::to_underlying(button)] = status; }

void Joypad::Write(uint8_t value) {
  m_strobe = value & 0b1;
  if (m_strobe) {
    m_button_index = 0;
  }
}
uint8_t Joypad::Read() {
  if (m_button_index > 7) {
    return 1;
  }

  auto result = uint8_t(m_button_status[m_button_index]);
  if (!m_strobe && m_button_index < 8) {
    ++m_button_index;
  }

  return result;
}
} // namespace BNES::HW