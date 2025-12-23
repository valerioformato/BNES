//
// Created by vformato on 10/17/25.
//

#include "Tools/CPUDebugger.h"
#include "SDLBind/Text/TextSpec.h"

#include <algorithm>
#include <ranges>

namespace BNES::Tools {

CPUDebugger::Window::Window(SDL::Buffer &&buffer)
    : m_window(SDL::Window::FromSpec(SDL::WindowSpec{
                                         .width = buffer.Width(),
                                         .height = buffer.Height(),
                                         .title = "CPU Debugger",
                                         .flags = SDL::WindowFlag::None,
                                     })
                   .value()),
      m_texture(m_window.CreateTexture(std::move(buffer)).value()),
      m_font(SDL::Font::Get("SpaceMono", SDL::FontVariant::Regular).value()) {}

ErrorOr<void> CPUDebugger::Window::Update(std::span<SDL::TextSpec> text_content) {
  SDL_RenderClear(m_window.Renderer());

  auto maybe_textures = text_content | std::views::transform([this](SDL::TextSpec spec) {
                          return SDL::Texture::FromText(m_window.Renderer(), spec);
                        });

  unsigned int y_pos = 0;
  std::ranges::for_each(maybe_textures, [this, &y_pos](auto &&maybe_texture) {
    if (maybe_texture.has_value()) {
      maybe_texture->RenderAtPosition(m_window.Renderer(), {0, y_pos});
      y_pos += m_font.LineSkip();
    }
  });

  SDL_RenderPresent(m_window.Renderer());

  return {};
}

void CPUDebugger::Update() {
  using CPU = HW::CPU;

  std::vector<std::string> lines;
  lines.push_back(fmt::format("A: {} X: {} Y: {}", m_cpu->Registers()[CPU::Register::A],
                              m_cpu->Registers()[CPU::Register::X], m_cpu->Registers()[CPU::Register::Y],
                              CPU::DisassembleInstruction(m_cpu->CurrentInstruction())));
  lines.push_back(fmt::format("{}", CPU::DisassembleInstruction(m_cpu->CurrentInstruction())));

  auto text_content = lines | std::views::transform([this](auto &&line) {
                        return SDL::TextSpec{
                            .content = line,
                            .font = m_window.m_font,
                            .color = SDL::Color{255, 255, 255, 255},
                        };
                      }) |
                      std::ranges::to<std::vector<SDL::TextSpec>>();

  m_window.Update(text_content);
}

} // namespace BNES::Tools
