//
// Created by vformato on 10/17/25.
//

#ifndef PPUDEBUGGER_H
#define PPUDEBUGGER_H

#include "HW/PPU.h"
#include "SDLBind/Graphics/Window.h"
#include "common/Types/non_owning_ptr.h"

namespace BNES::Tools {

class PPUDebugger {
public:
  struct Window {
    explicit Window(SDL::Buffer &&buffer);

    ErrorOr<void> Update();

    SDL::Window m_window;
    SDL::Font m_font;
  };

  PPUDebugger() = delete;
  explicit PPUDebugger(const HW::PPU &ppu) : m_ppu(&ppu), m_window(SDL::Buffer::FromSize(800, 600).value()) {}

  [[nodiscard]] ErrorOr<void> SetPosition(unsigned int x, unsigned int y) const {
    return m_window.m_window.SetPosition(x, y);
  };

  void Present() const { m_window.m_window.Present(); }

  void Update();

private:
  non_owning_ptr<const HW::PPU *> m_ppu;

  Window m_window;
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
