//
// Created by vformato on 10/17/25.
//

#ifndef CPUDEBUGGER_H
#define CPUDEBUGGER_H

#include "HW/CPU.h"
#include "SDLBind/Graphics/Window.h"
#include "common/Types/non_owning_ptr.h"

namespace BNES::Tools {

class CPUDebugger {
public:
  CPUDebugger() = delete;
  explicit CPUDebugger(const HW::CPU &cpu)
      : m_cpu(&cpu), m_window(SDL::Window::FromSpec(SDL::WindowSpec{
                                                        .width = 800,
                                                        .height = 600,
                                                        .title = "CPU Debugger",
                                                        .flags = SDL::WindowFlag::None,
                                                    })
                                  .value()),
        m_font(SDL::Font::Get("SpaceMono", SDL::FontVariant::Regular).value()) {}

  [[nodiscard]] SDL::Window &GetWindow() { return m_window; }

  ErrorOr<void> Update();

private:
  non_owning_ptr<const HW::CPU *> m_cpu;

  SDL::Window m_window;
  SDL::Font m_font;
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
