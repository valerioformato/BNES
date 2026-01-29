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

    ErrorOr<void> Update(SDL::Texture &chr_rom_tex);

    SDL::Window m_window;
  };

  PPUDebugger() = delete;
  explicit PPUDebugger(const HW::PPU &ppu)
      : m_ppu(&ppu), m_window(SDL::Buffer::FromSize(800, 600).value()),
        m_chr_rom_texture(
            SDL::Texture::FromBuffer(m_window.m_window.Renderer(), SDL::Buffer::FromSize(128, 256).value()).value()) {}

  [[nodiscard]] SDL::Window &GetWindow() { return m_window.m_window; }

  ErrorOr<void> Update();

private:
  non_owning_ptr<const HW::PPU *> m_ppu;

  Window m_window;
  SDL::Texture m_chr_rom_texture;
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
