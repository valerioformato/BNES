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
      : m_cpu(&cpu), m_font(SDL::Font::Get("SpaceMono", SDL::FontVariant::Regular).value()) {}

  [[nodiscard]] ErrorOr<SDL::Texture> BuildTexture(const SDL::Window &main_window);

private:
  non_owning_ptr<const HW::CPU *> m_cpu;

  SDL::Font m_font;
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
