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
  // TODO: Make the window a popup window with the main emulator window as its parent. This should allow us to
  // reposition it also if running under Wayland.
  CPUDebugger() = delete;
  explicit CPUDebugger(const HW::CPU &cpu, bool should_focus = false)
      : m_cpu(&cpu), m_window(SDL::Window::FromSpec(SDL::WindowSpec{
                                                        .width = 800,
                                                        .height = 600,
                                                        .title = "CPU Debugger",
                                                        .flags = SDL::WindowFlag::None,
                                                        .should_steal_focus = should_focus,
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
