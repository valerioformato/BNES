//
// Created by vformato on 10/17/25.
//

#ifndef CPUDEBUGGER_H
#define CPUDEBUGGER_H

#include "../SDLBind/Graphics/Window.h"
#include "HW/CPU.h"
#include "common/Types/non_owning_ptr.h"

namespace BNES::Tools {

class CPUDebugger {
public:
  class Window {
  public:
    explicit Window(SDL::Buffer &&buffer);

    ErrorOr<void> Update(SDL::TextSpec text_content);

    SDL::Window m_window;
    // SDL::Texture m_texture;
    SDL::Font m_font;
  };

  CPUDebugger() = delete;
  explicit CPUDebugger(const HW::CPU &cpu) : m_cpu(&cpu), m_window(SDL::Buffer::FromSize(800, 600).value()) {}

  [[nodiscard]] ErrorOr<void> SetPosition(unsigned int x, unsigned int y) const {
    return m_window.m_window.SetPosition(x, y);
  };

  void Update();

private:
  non_owning_ptr<const HW::CPU *> m_cpu;

  Window m_window;
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
