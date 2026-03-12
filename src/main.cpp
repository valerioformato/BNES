#include "HW/Bus.h"
#include "HW/CPU.h"
#include "HW/Constants.h"
#include "HW/PPU.h"
#include "HW/Screen.h"
#include "SDLBind/Graphics/Buffer.h"
#include "SDLBind/Graphics/Window.h"
#include "SDLBind/Init.h"
#include "Tools/CPUDebugger.h"
#include "Tools/PPUDebugger.h"

#include <cxxopts.hpp>

#include <string>

// Frees media and shuts down SDL
void close() {
  // Quit SDL subsystems
  SDL_Quit();
}

namespace fs = std::filesystem;

struct Options {
  std::string rom_path{};
  bool batch{false};
  bool stepping{false};
};

BNES::ErrorOr<int> nes_main(Options options) { // Final exit code
  // Initialize SDL
  if (auto result = BNES::SDL::Init(); !result) {
    spdlog::error("Unable to initialize program!");
    return 1;
  }

  BNES::HW::Bus bus;
  TRY(bus.LoadRom(options.rom_path));

  BNES::HW::CPU cpu{bus};
  cpu.Init();

  BNES::HW::PPU ppu{bus};

  constexpr unsigned int MAIN_WINDOW_W = BNES::HW::NES_SCREEN_W * 6 + 20;
  constexpr unsigned int MAIN_WINDOW_H = BNES::HW::NES_SCREEN_H * 4;

  constexpr double BNES_TARGET_FPS = 60.0;

  // Create the main screen window
  BNES::SDL::Window main_window = TRY(BNES::SDL::Window::FromSpec({
      .width = MAIN_WINDOW_W,
      .height = MAIN_WINDOW_H,
      .title = "BNES",
      .should_steal_focus = true,
  }));
  main_window.Present();

  BNES::HW::Screen screen{bus};
  TRY(screen.Init(main_window));

  BNES::Tools::CPUDebugger cpu_debugger(cpu);
  BNES::Tools::PPUDebugger ppu_debugger(ppu);

  // Main event loop
  auto time_point = std::chrono::system_clock::now();

  while (true) {
    bool proceed = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
        case SDLK_S:
          if (!options.stepping) {
            options.stepping = true;
            spdlog::info("Single stepping enabled. Press 's' to step through instructions.");
          } else {
            proceed = true;
          }
          break;
        case SDLK_C:
          if (options.stepping) {
            options.stepping = false;
            spdlog::info("Single stepping disabled. Execution continues.");
          }
          break;
        case SDLK_Q:
        case SDLK_ESCAPE:
          spdlog::info("Quit requested");
          BNES::SDL::Quit();
          return 0;
        }
        break;
      case SDL_EVENT_QUIT:
        spdlog::info("Quit requested");
        BNES::SDL::Quit();
        return 0;
      }
    }

    // If stepping is enabled and we haven't received a proceed signal, wait for events
    if (options.stepping && !proceed) {
      SDL_Delay(10); // Small delay to avoid busy-waiting
      continue;
    }

    // Check if we hit a BRK instruction (opcode 0x00) - stop execution
    uint8_t opcode = bus.Read(cpu.ProgramCounter());
    if (opcode == 0x00) {
      break;
    }

    constexpr auto frame_cycles = BNES::HW::NES_CPU_FREQ_HZ / BNES_TARGET_FPS;
    const auto target_cpu_cycles = options.stepping ? cpu.Cycles() + 1 : cpu.Cycles() + frame_cycles;

    auto time_since_last_frame_update = std::chrono::system_clock::now() - time_point;

    if (time_since_last_frame_update > std::chrono::microseconds(16667)) {
      time_point = std::chrono::system_clock::now();

      main_window.Clear();

      TRY(screen.DrawScreen(main_window, 4.0f));

      // Draw CPU debug info
      TRY(cpu_debugger.BuildTexture(main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPosition(main_window.Renderer(), {BNES::HW::NES_SCREEN_W * 4, 0});
      }));

      // Draw PPU debug info
      TRY(ppu_debugger.BuildChrRomTexture(main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPositionAndScale(main_window.Renderer(),
                                            {BNES::HW::NES_SCREEN_W * 4, BNES::HW::NES_SCREEN_H * 3}, 2.0f);
      }));
      TRY(ppu_debugger.BuildPPURegisterText(main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPosition(main_window.Renderer(), {BNES::HW::NES_SCREEN_W * 4, BNES::HW::NES_SCREEN_H * 2});
      }));
      TRY(ppu_debugger.BuildPaletteTexture(main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPosition(main_window.Renderer(), {BNES::HW::NES_SCREEN_W * 5, BNES::HW::NES_SCREEN_H * 2});
      }));

      main_window.Present();

      while (cpu.Cycles() < target_cpu_cycles) {
        cpu.RunInstruction(cpu.DecodeNextInstruction());
      }
    }
  }

  // Clean up
  BNES::SDL::Quit();

  return 0;
}

int main(int argc, char **argv) {
  cxxopts::Options options("nestest", "Run the NESTEST rom and dump CPU status");

  // clang-format off
  options.add_options()
    ("s,stepping", "Start with single stepping enabled")
    ("v,verbose", "Verbosity level (use -v for Debug, -vv for Trace)", cxxopts::value<int>()->default_value("0")->implicit_value("1"))
    ("romfile", "ROM to load", cxxopts::value<std::string>())
    ("version", "Print version information")
    ("h,help", "Print usage");
  // clang-format on

  options.parse_positional({"romfile"});

  try {
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
      fmt::println("{}", options.help());
      fmt::println("\nControls:");
      fmt::println("  s         Toggle/step: Enable stepping mode, or step one instruction");
      fmt::println("  c         Continue: Disable stepping mode and resume execution");
      fmt::println("  q/ESC     Quit the program");
      return 0;
    }

    if (result.count("version")) {
      fmt::println("nestest v0.0.0");
      return 0;
    }

    int verbosity = result["verbose"].count();
    switch (verbosity) {
    case 1:
      spdlog::set_level(spdlog::level::debug);
      break;
    case 2:
      spdlog::set_level(spdlog::level::trace);
      break;
    }

    auto main_result = nes_main({
        .rom_path = result["romfile"].as<std::string>(),
        .stepping = result["stepping"].as<bool>(),
    });

    if (!main_result) {
      spdlog::error("Error: {}", main_result.error().Message());
      return main_result.error().Code().value();
    }

    return 0;
  } catch (const cxxopts::exceptions::exception &e) {
    fmt::println("Error parsing options: {}", e.what());
    return 1;
  }
}
