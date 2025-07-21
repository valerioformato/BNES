#ifndef BNES_HW_CPU_H
#define BNES_HW_CPU_H

#include "common/EnumArray.h"

#include <cstdint>

namespace BNES::HW {
class CPU {
  enum class Register : uint8_t { A = 0, X, Y };

private:
  EnumArray<uint8_t, Register> m_registers{};
};
} // namespace BNES::HW

#endif
