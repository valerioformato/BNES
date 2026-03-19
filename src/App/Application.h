#ifndef BNES_APP_H
#define BNES_APP_H

#include "App/Event.h"
#include "HW/Bus.h"
#include "HW/CPU.h"
#include "HW/PPU.h"
#include "HW/Screen.h"
#include "SDLBind/Graphics/Window.h"
#include "SDLBind/OS/Event.h"
#include "Tools/CPUDebugger.h"
#include "Tools/PPUDebugger.h"
#include "spdlog/sinks/stdout_color_sinks.h"

namespace BNES {
class App {
public:
  struct Options {
    std::string rom_path{};
    bool batch{false};
    bool stepping{false};
  };

  explicit App(Options options) : m_options{std::move(options)}, m_logger{spdlog::stdout_color_st("App")} {}

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

  bool m_can_step{false};

  std::shared_ptr<spdlog::logger> m_logger;
  ErrorOr<Event> FromSDL(SDL::Event event);
  ErrorOr<void> HandleEvent(Event event);

public:
  // event handlers
  void operator()(StepEvent event);
  void operator()(ContinueEvent event);
  void operator()(QuitEvent event);
  void operator()(std::monostate);
};
} // namespace BNES

#endif
