//
// Created by Valerio Formato on 26-Feb-26.
//

#ifndef BNES_SCREEN_H
#define BNES_SCREEN_H

#include "HW/PPU.h"
#include "SDLBind/Graphics/Texture.h"
#include "SDLBind/Graphics/Window.h"

namespace BNES::HW {

class Screen {
public:
  static constexpr unsigned int NES_SCREEN_W = 256;
  static constexpr unsigned int NES_SCREEN_H = 240;

  Screen() = delete;
  explicit Screen(Bus &bus) { bus.Attach(this); };

  ErrorOr<void> Init(const SDL::Window &window);

  ErrorOr<void> FillFromPPU(const PPU &ppu);
  ErrorOr<void> FillBackground(const PPU &ppu);
  ErrorOr<void> FillSprites(const PPU &ppu);

  ErrorOr<void> DrawScreen(SDL::Window &window, float scale_factor = 1);

private:
  SDL::Texture m_texture;
};

} // namespace BNES::HW

#endif // BNES_SCREEN_H
