//
// Created by vformato on 10/17/25.
//

#include "Tools/CPUDebugger.h"
#include "SDLBind/Text/TextSpec.h"

namespace BNES::Tools {

CPUDebugger::Window::Window(SDL::Buffer &&buffer)
    : m_window(SDL::Window::FromSpec(SDL::Window::WindowSpec{
                                         .width = buffer.Width(),
                                         .height = buffer.Height(),
                                         .title = "CPU Debugger",
                                         .flags = SDL::WindowFlag::None,
                                     })
                   .value()),
      m_texture(m_window.CreateTexture(std::move(buffer)).value()),
      m_font(SDL::Font::Get("SpaceMono", SDL::FontVariant::Regular).value()) {}

ErrorOr<void> CPUDebugger::Window::Update(SDL::TextSpec text_content) {
  SDL_RenderClear(m_window.Renderer());

  m_texture = TRY(SDL::Texture::FromText(m_window.Renderer(), text_content));

  m_texture.Render(m_window.Renderer());

  SDL_RenderPresent(m_window.Renderer());

  return {};
}

void CPUDebugger::Update() {
  using CPU = HW::CPU;

  const std::string register_debug =
      fmt::format("A: {} X: {} Y: {}", m_cpu->Registers()[CPU::Register::A], m_cpu->Registers()[CPU::Register::X],
                  m_cpu->Registers()[CPU::Register::Y]);

  const SDL::TextSpec content{
      .content = register_debug,
      .font = m_window.m_font,
      .color = SDL::Color{255, 255, 255, 255},
  };

  m_window.Update(content);
}

} // namespace BNES::Tools
