//
// Created by vformato on 9/16/25.
//

#include "../../src/SDLBind/Graphics/Buffer.h"
#include "../../src/SDLBind/Graphics/Window.h"
#include "HW/CPU.h"
#include "SDLBind/Init.h"
#include "Tools/CPUDebugger.h"
#include "common/Time.h"

#include <algorithm>
#include <iostream>
#ifndef _MSC_VER
#include <cxxabi.h>
#endif
class SnakeCPU : public BNES::HW::CPU {
public:
  using CPU::ReadFromMemory;
  using CPU::SetProgramStartAddress;
  using CPU::WriteToMemory;

  SnakeCPU(BNES::HW::Bus &bus) : CPU(bus) {}

  void RunOneInstruction() {
    static constexpr auto cpu_freq = std::chrono::duration<double>(1.0f / 1790000.0f); // 1.79 MHz

    // update the RNG at address $FE
    WriteToMemory(0xFE, std::rand() % 256);

    std::array<uint8_t, 3> bytes{};
    for (size_t i = 0; i < 3; ++i) {
      bytes[i] = ReadFromMemory(ProgramCounter() + i);
    }
    auto instr = DecodeInstruction(bytes);
    auto instr_disass = DisassembleInstruction(instr);
    unsigned int instr_cycles = std::visit([](DecodedInstruction &arg) { return arg.cycles; }, instr);
    spdlog::trace("A: {:02X} X: {:02X} Y: {:02X} SP: {:02X} Status: {}", Registers()[Register::A],
                  Registers()[Register::X], Registers()[Register::Y], StackPointer(), StatusFlags().to_string());
    spdlog::trace("PC: {:04X} Opcode: {:02X} Instruction: {}", ProgramCounter(), bytes[0], instr_disass);

    RunInstruction(std::move(instr));

    BNES::Chrono::sleep_for(cpu_freq * instr_cycles * instruction_slowdown);
  }

  double instruction_slowdown{1.0};
};

BNES::ErrorOr<int> snake_main() {
  // spdlog::set_level(spdlog::level::trace);

  // SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

  auto clock = std::chrono::high_resolution_clock{};
  static constexpr auto frame_duration = std::chrono::duration<double>(1.0f / 60.0f);

  // Initialize SDL
  if (auto result = BNES::SDL::Init(); !result) {
    spdlog::error("Unable to initialize program!");
    return 1;
  }

  auto window_handle = TRY(BNES::SDL::Window::FromSpec({.width = 320, .height = 320, .title = "6502 Snake"}));
  auto screen_surface = window_handle.Surface();

  auto buffer = TRY(BNES::SDL::Buffer::FromSize(32, 32));
  auto texture = TRY(window_handle.CreateTexture(std::move(buffer)));

  window_handle.SetRenderScale(10.0f, 10.0f);
  texture.SetScaleMode(SDL_ScaleMode::SDL_SCALEMODE_NEAREST); // Pixelated scaling

  // The quit flag
  bool quit{false}, step{false}, stepping{false};

  // The event data
  SDL_Event e;
  SDL_zero(e);

  BNES::HW::Bus bus;
  TRY(bus.LoadRom("assets/roms/snake.nes"));

  SnakeCPU cpu{bus};
  cpu.Init();

  auto [main_window_x, main_window_y] = window_handle.Position();
  auto [main_window_size_x, main_window_size_y] = window_handle.Size();

  BNES::Tools::CPUDebugger debugger(cpu);
  // This doesn't work on Wayland cause applications cannot request absolute window positioning
  if (auto result = getenv("WAYLAND_DISPLAY"); result == nullptr) {
    TRY(debugger.SetPosition(main_window_x + main_window_size_x, main_window_y));
  } else {
    spdlog::warn("Cannot set debugger window position on Wayland ({})", result);
  }

  cpu.instruction_slowdown = 100.0;

  auto update_video_buffer = [&cpu](BNES::SDL::Buffer &target_buffer) {
    auto buffer_pixels = target_buffer.Pixels();

    static constexpr uint16_t start_address{0x200};
    std::ranges::generate(buffer_pixels, [&cpu, addr = start_address]() mutable {
      switch (cpu.ReadFromMemory(addr++) & 0x0F) {
      case 0:
        return BNES::SDL::Pixel{0, 0, 0, 255}; // Black
      case 1:
        return BNES::SDL::Pixel{255, 255, 255, 255}; // White
      case 2:
      case 9:
        return BNES::SDL::Pixel{128, 128, 128, 255}; // Gray
      case 3:
      case 10:
        return BNES::SDL::Pixel{255, 0, 0, 255}; // Red
      case 4:
      case 11:
        return BNES::SDL::Pixel{0, 255, 0, 255}; // Green
      case 5:
      case 12:
        return BNES::SDL::Pixel{0, 0, 255, 255}; // Blue
      case 6:
      case 13:
        return BNES::SDL::Pixel{255, 0, 255, 255}; // Magenta
      case 7:
      case 14:
        return BNES::SDL::Pixel{255, 255, 0, 255}; // Yellow
      default:
        return BNES::SDL::Pixel{0, 255, 255, 255}; // Cyan
      };
    });
  };

  auto dump_snake_data = [&cpu]() {
    spdlog::debug("Snake: head: ({}, {}), tail: ({}, {}), length: {}", cpu.ReadFromMemory(0x10),
                  cpu.ReadFromMemory(0x11), cpu.ReadFromMemory(0x14), cpu.ReadFromMemory(0x15),
                  cpu.ReadFromMemory(0x03));
  };

  auto dump_apple_data = [&cpu]() {
    auto apple_color = cpu.ReadFromMemory(cpu.ReadFromMemory(0x00) | (cpu.ReadFromMemory(0x01) << 8));
    auto apple_loc = std::make_pair(cpu.ReadFromMemory(0x00), cpu.ReadFromMemory(0x01));
    spdlog::info("Apple: ({}, {}) color: {:X}", cpu.ReadFromMemory(0x00), cpu.ReadFromMemory(0x01), apple_color);

    return apple_loc.first | (apple_loc.second << 8);
  };

  // The main loop
  std::chrono::time_point last_frame_update_time = clock.now();
  while (quit == false) {
    auto begin = clock.now();

    // Get event data
    while (SDL_PollEvent(&e) == true) {
      // If event is quit type
      if (e.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        // End the main loop
        quit = true;
      }

      if (e.type == SDL_EVENT_KEY_DOWN) {
        switch (e.key.key) {
        case SDLK_UP:
          cpu.WriteToMemory(0xFF, 0x77);
          break;
        case SDLK_DOWN:
          cpu.WriteToMemory(0xFF, 0x73);
          break;
        case SDLK_LEFT:
          cpu.WriteToMemory(0xFF, 0x61);
          break;
        case SDLK_RIGHT:
          cpu.WriteToMemory(0xFF, 0x64);
          break;
        case SDLK_S:
          step = true;
          stepping = true;
          break;
        }
      }
    }

    if (stepping) {
      try {
        if (step) {
          cpu.RunOneInstruction();
          step = false;
        }
      } catch (std::exception &e) {
        spdlog::error("unexpected error: {}", e.what());
        quit = true;
      }
    } else {
      cpu.RunOneInstruction();
    }

    if (begin - last_frame_update_time > frame_duration) {
      // auto apple_address = dump_apple_data();
      // dump_snake_data();

      // debugger.Update();

      update_video_buffer(texture.Buffer());

      TRY(texture.Update());

      // SDL_RenderClear(window_handle.Renderer());

      texture.Render(window_handle.Renderer());

      SDL_RenderPresent(window_handle.Renderer());

      debugger.Update();

      last_frame_update_time = clock.now();
      // spdlog::debug("new frame!");
    }

    // or wait until we press enter
    // std::cin.get();
  }

  // Clean up
  BNES::SDL::Quit();

  return 0;
}

int main() {
  auto result = snake_main();
  if (!result) {
    spdlog::error("Error: {}", result.error().Message());
    return result.error().Code().value();
  }

  return 0;
}
