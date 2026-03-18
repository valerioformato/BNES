#ifndef BNES_APP_H
#define BNES_APP_H

#include "HW/Bus.h"
#include "HW/CPU.h"
#include "HW/PPU.h"
#include "HW/Screen.h"
#include "SDLBind/Graphics/Window.h"
#include "Tools/CPUDebugger.h"
#include "Tools/PPUDebugger.h"

namespace BNES {
class App {
public:
  struct Options {
    std::string rom_path{};
    bool batch{false};
    bool stepping{false};
  };

  explicit App(Options options) : m_options{std::move(options)} {}

  ErrorOr<void> Run();

private:
  Options m_options;

  HW::Bus m_bus;
  HW::CPU m_cpu{m_bus};
  HW::PPU m_ppu{m_bus};
  HW::Screen m_screen{m_bus};

  Tools::CPUDebugger m_cpu_debugger{m_cpu};
  Tools::PPUDebugger m_ppu_debugger{m_ppu};

  SDL::Window m_main_window;
};
} // namespace BNES

#endif
