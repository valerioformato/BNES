#include "App/Application.h"
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
    default:
      break;
    }

    // Initialize SDL
    if (auto result = BNES::SDL::Init(); !result) {
      spdlog::error("Unable to initialize program!");
      return 1;
    }

    BNES::App application{{
        .rom_path = result["romfile"].as<std::string>(),
        .stepping = result["stepping"].as<bool>(),
    }};

    auto main_result = application.Run();

    if (!main_result) {
      spdlog::error("Error: {}", main_result.error().Message());
      return main_result.error().Code().value();
    }

    // Clean up
    BNES::SDL::Quit();

    return 0;
  } catch (const cxxopts::exceptions::exception &e) {
    fmt::println("Error parsing options: {}", e.what());
    return 1;
  }
}
