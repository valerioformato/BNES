//
// Created by vformato on 10/17/25.
//

#ifndef CPUDEBUGGER_H
#define CPUDEBUGGER_H

#include "HW/CPU.h"
#include "SDLBind/WindowHandle.h"
#include "common/Types/non_owning_ptr.h"

namespace BNES::Tools {

class CPUDebugger {
public:
  class Window {
  public:
    explicit Window(SDL::Buffer &&buffer);

    void Update();

    SDL::WindowHandle m_window_handle;
    SDL::TextureHandle m_texture;
  };

public:
  CPUDebugger() = delete;
  explicit CPUDebugger(const HW::CPU &cpu) : m_cpu(&cpu), m_window(SDL::MakeBuffer(800, 600).value()) {}

  void Update();

private:
  non_owning_ptr<const HW::CPU *> m_cpu;

  Window m_window;
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
