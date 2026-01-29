//
// Created by vformato on 10/17/25.
//

#include "Tools/PPUDebugger.h"
#include "SDLBind/Text/TextSpec.h"

#include <algorithm>
#include <numeric>
#include <ranges>

namespace BNES::Tools {

PPUDebugger::Window::Window(SDL::Buffer &&buffer)
    : m_window(SDL::Window::FromSpec(SDL::WindowSpec{
                                         .width = buffer.Width(),
                                         .height = buffer.Height(),
                                         .title = "PPU Debugger",
                                         .flags = SDL::WindowFlag::None,
                                     })
                   .value()),
      m_font(SDL::Font::Get("SpaceMono", SDL::FontVariant::Regular).value()) {}

ErrorOr<void> PPUDebugger::Window::Update() {
  SDL_RenderClear(m_window.Renderer());

  // auto texture = TRY(SDL::Texture::FromText(m_window.Renderer(), text_content));
  // TRY(texture.Render(m_window.Renderer()));

  SDL_RenderPresent(m_window.Renderer());

  return {};
}

void PPUDebugger::Update() {
  using PPU = HW::PPU;

  TODO("Implement drawing of PPU internal memory");

  m_window.Update();
}

} // namespace BNES::Tools
