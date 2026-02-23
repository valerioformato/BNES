#include "HW/Bus.h"
#include "HW/CPU.h"
#include "HW/PPU.h"
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

  // Create the main screen window
  BNES::SDL::Window main_window = TRY(BNES::SDL::Window::CreateDefault());
  main_window.Present();

  BNES::Tools::CPUDebugger cpu_debugger(cpu);
  TRY(cpu_debugger.CreatePopupWindow({.width = 640, .height = 480, .title = "CPU Debugger"}, main_window));
  cpu_debugger.GetWindow().Present();

  // BNES::Tools::PPUDebugger ppu_debugger(ppu);
  // ppu_debugger.GetWindow().SetRenderScale(2, 2);
  // ppu_debugger.GetWindow().Present();

  // auto cpu_d_pos = cpu_debugger.GetWindow().Position();
  // auto ppu_d_pos = ppu_debugger.GetWindow().Position();
  //
  // cpu_d_pos[0] -= cpu_debugger.GetWindow().Size()[0] / 2;
  // ppu_d_pos[0] += cpu_debugger.GetWindow().Size()[0] / 2;
  //
  // TRY(cpu_debugger.GetWindow().SetPosition(cpu_d_pos[0], cpu_d_pos[1]));
  // TRY(ppu_debugger.GetWindow().SetPosition(ppu_d_pos[0], ppu_d_pos[1]));

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

    cpu.RunInstruction(cpu.DecodeNextInstruction());

    auto time_since_last_frame_update = std::chrono::system_clock::now() - time_point;

    if (time_since_last_frame_update > std::chrono::microseconds(16667)) {
      TRY(cpu_debugger.Update());
      // TRY(ppu_debugger.Update());
      time_point = std::chrono::system_clock::now();
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
