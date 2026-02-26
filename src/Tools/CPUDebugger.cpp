//
// Created by vformato on 10/17/25.
//

#include "Tools/CPUDebugger.h"
#include "SDLBind/Text/TextSpec.h"

#include <numeric>

namespace BNES::Tools {

ErrorOr<SDL::Texture> CPUDebugger::BuildTexture(const SDL::Window &main_window) {
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

  std::string content = std::ranges::fold_left(
      lines, std::string{}, [](auto &&current, auto &&text) { return fmt::format("{}{}\n", current, text); });

  SDL::TextSpec text_content{
      .content = content,
      .font = m_font,
      .color = SDL::Color{255, 255, 255, 255},
      .wrapping = SDL::TextWrapping::Wrapped,
      .wrap_size = static_cast<unsigned int>(main_window.Size()[1] / 2),
  };

  return SDL::Texture::FromText(main_window.Renderer(), text_content);
}

} // namespace BNES::Tools
