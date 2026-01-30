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
  PPUDebugger() = delete;
  explicit PPUDebugger(const HW::PPU &ppu)
      : m_ppu(&ppu), m_window(SDL::Window::FromSpec(SDL::WindowSpec{
                                                        .width = 800,
                                                        .height = 600,
                                                        .title = "CPU Debugger",
                                                        .flags = SDL::WindowFlag::None,
                                                    })
                                  .value()),
        m_chr_rom_texture(
            SDL::Texture::FromBuffer(m_window.Renderer(), SDL::Buffer::FromSize(128, 256).value()).value()) {}

  [[nodiscard]] SDL::Window &GetWindow() { return m_window; }

  ErrorOr<void> Update();

private:
  non_owning_ptr<const HW::PPU *> m_ppu;

  SDL::Window m_window;

  SDL::Texture m_chr_rom_texture;
  ErrorOr<void> UpdateChrRomTexture();
};

} // namespace BNES::Tools

#endif // CPUDEBUGGER_H
