#include "HW/CPU.h"

namespace BNES::HW {
ErrorOr<void> CPU::LoadProgram(std::vector<uint8_t> program) {
  if (program.size() > m_program_memory.size()) {
    return make_error(std::make_error_code(std::errc::not_enough_memory), "Program too large to fit in memory");
  }
  std::ranges::copy(program, m_program_memory.begin());

  return {};
}
} // namespace BNES::HW
