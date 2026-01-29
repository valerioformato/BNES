//
// Created by vformato on 10/17/25.
//

#include "Tools/CPUDebugger.h"
#include "SDLBind/Text/TextSpec.h"

#include <algorithm>
#include <numeric>
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
      m_font(SDL::Font::Get("SpaceMono", SDL::FontVariant::Regular).value()) {}

ErrorOr<void> CPUDebugger::Window::Update(SDL::TextSpec text_content) {
  SDL_RenderClear(m_window.Renderer());

  auto texture = TRY(SDL::Texture::FromText(m_window.Renderer(), text_content));
  TRY(texture.Render(m_window.Renderer()));

  SDL_RenderPresent(m_window.Renderer());

  return {};
}

ErrorOr<void> CPUDebugger::Update() {
  using CPU = HW::CPU;

  std::vector<std::string> lines;
  lines.push_back(fmt::format("A: {} X: {} Y: {}", m_cpu->Registers()[CPU::Register::A],
                              m_cpu->Registers()[CPU::Register::X], m_cpu->Registers()[CPU::Register::Y],
                              m_cpu->DisassembleInstruction(m_cpu->CurrentInstruction())));
  lines.push_back(
      fmt::format("PC: {:4X} {}", m_cpu->ProgramCounter(), m_cpu->DisassembleInstruction(m_cpu->CurrentInstruction())));

  lines.push_back("");
  lines.push_back("              NV1BDIZC");
  lines.push_back(fmt::format("Status Flags: {}", m_cpu->StatusFlags().to_string()));

  std::string content = std::reduce(begin(lines), end(lines), std::string{},
                                    [](auto &&current, auto &&text) { return fmt::format("{}{}\n", current, text); });

  SDL::TextSpec text_content{
      .content = content,
      .font = m_window.m_font,
      .color = SDL::Color{255, 255, 255, 255},
      .wrapping = SDL::TextWrapping::Wrapped,
      .wrap_size = static_cast<unsigned int>(m_window.m_window.Size()[1]),
  };

  return m_window.Update(text_content);
}

} // namespace BNES::Tools
