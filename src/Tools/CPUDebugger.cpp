//
// Created by vformato on 10/17/25.
//

#include "CPUDebugger.h"

namespace BNES::Tools {

ErrorOr<void> CPUDebugger::InitWindow() {
  m_window_handle = TRY(SDL::MakeWindow(SDL::WindowHandle::WindowSpec{
      .title = "CPU Debugger",
      .width = 800,
      .height = 600,
      .flags = SDL::WindowFlag::Resizable,
  }));

  return {};
}

} // namespace BNES::Tools
