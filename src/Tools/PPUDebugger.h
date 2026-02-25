//
// Created by vformato on 10/17/25.
//

#ifndef PPUDEBUGGER_H
#define PPUDEBUGGER_H

#include "HW/PPU.h"
#include "SDLBind/Graphics/Texture.h"
#include "SDLBind/Graphics/Window.h"
#include "common/Types/non_owning_ptr.h"

namespace BNES::Tools {

class PPUDebugger {
public:
  PPUDebugger() = delete;
  explicit PPUDebugger(const HW::PPU &ppu) : m_ppu(&ppu) {}

  ErrorOr<void> Update();

  ErrorOr<SDL::Texture> BuildChrRomTexture(const SDL::Window &main_window);

private:
  non_owning_ptr<const HW::PPU *> m_ppu;
};

} // namespace BNES::Tools

#endif // PPUDEBUGGER_H
