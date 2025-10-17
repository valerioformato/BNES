//
// Created by vformato on 10/17/25.
//

#ifndef CPUDEBUGGER_H
#define CPUDEBUGGER_H

#include "HW/CPU.h"
#include "SDLBind/WindowHandle.h"

namespace BNES {
namespace Tools {

class CPUDebugger {
public:
  ErrorOr<void> InitWindow();

  void AttachToCPU(HW::CPU &cpu);

private:
  SDL::WindowHandle m_window_handle;

  // std::shared_ptr<HW::CPU> m_cpu{nullptr};
};

} // namespace Tools
} // namespace BNES

#endif // CPUDEBUGGER_H
