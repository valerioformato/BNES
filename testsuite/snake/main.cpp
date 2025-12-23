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

  BNES::ErrorOr<void> LoadProgram(std::span<const uint8_t> program) {
    uint16_t start_address = 0x600;
    for (const auto &byte : program) {
      WriteToMemory(start_address++, byte);
    }

    return {};
  }

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
  constexpr std::array<uint8_t, 309> program = {
      0x20, 0x06, 0x06, 0x20, 0x38, 0x06, 0x20, 0x0d, 0x06, 0x20, 0x2a, 0x06, 0x60, 0xa9, 0x02, 0x85, 0x02, 0xa9, 0x04,
      0x85, 0x03, 0xa9, 0x11, 0x85, 0x10, 0xa9, 0x10, 0x85, 0x12, 0xa9, 0x0f, 0x85, 0x14, 0xa9, 0x04, 0x85, 0x11, 0x85,
      0x13, 0x85, 0x15, 0x60, 0xa5, 0xfe, 0x85, 0x00, 0xa5, 0xfe, 0x29, 0x03, 0x18, 0x69, 0x02, 0x85, 0x01, 0x60, 0x20,
      0x4d, 0x06, 0x20, 0x8d, 0x06, 0x20, 0xc3, 0x06, 0x20, 0x19, 0x07, 0x20, 0x20, 0x07, 0x20, 0x2d, 0x07, 0x4c, 0x38,
      0x06, 0xa5, 0xff, 0xc9, 0x77, 0xf0, 0x0d, 0xc9, 0x64, 0xf0, 0x14, 0xc9, 0x73, 0xf0, 0x1b, 0xc9, 0x61, 0xf0, 0x22,
      0x60, 0xa9, 0x04, 0x24, 0x02, 0xd0, 0x26, 0xa9, 0x01, 0x85, 0x02, 0x60, 0xa9, 0x08, 0x24, 0x02, 0xd0, 0x1b, 0xa9,
      0x02, 0x85, 0x02, 0x60, 0xa9, 0x01, 0x24, 0x02, 0xd0, 0x10, 0xa9, 0x04, 0x85, 0x02, 0x60, 0xa9, 0x02, 0x24, 0x02,
      0xd0, 0x05, 0xa9, 0x08, 0x85, 0x02, 0x60, 0x60, 0x20, 0x94, 0x06, 0x20, 0xa8, 0x06, 0x60, 0xa5, 0x00, 0xc5, 0x10,
      0xd0, 0x0d, 0xa5, 0x01, 0xc5, 0x11, 0xd0, 0x07, 0xe6, 0x03, 0xe6, 0x03, 0x20, 0x2a, 0x06, 0x60, 0xa2, 0x02, 0xb5,
      0x10, 0xc5, 0x10, 0xd0, 0x06, 0xb5, 0x11, 0xc5, 0x11, 0xf0, 0x09, 0xe8, 0xe8, 0xe4, 0x03, 0xf0, 0x06, 0x4c, 0xaa,
      0x06, 0x4c, 0x35, 0x07, 0x60, 0xa6, 0x03, 0xca, 0x8a, 0xb5, 0x10, 0x95, 0x12, 0xca, 0x10, 0xf9, 0xa5, 0x02, 0x4a,
      0xb0, 0x09, 0x4a, 0xb0, 0x19, 0x4a, 0xb0, 0x1f, 0x4a, 0xb0, 0x2f, 0xa5, 0x10, 0x38, 0xe9, 0x20, 0x85, 0x10, 0x90,
      0x01, 0x60, 0xc6, 0x11, 0xa9, 0x01, 0xc5, 0x11, 0xf0, 0x28, 0x60, 0xe6, 0x10, 0xa9, 0x1f, 0x24, 0x10, 0xf0, 0x1f,
      0x60, 0xa5, 0x10, 0x18, 0x69, 0x20, 0x85, 0x10, 0xb0, 0x01, 0x60, 0xe6, 0x11, 0xa9, 0x06, 0xc5, 0x11, 0xf0, 0x0c,
      0x60, 0xc6, 0x10, 0xa5, 0x10, 0x29, 0x1f, 0xc9, 0x1f, 0xf0, 0x01, 0x60, 0x4c, 0x35, 0x07, 0xa0, 0x00, 0xa5, 0xfe,
      0x91, 0x00, 0x60, 0xa6, 0x03, 0xa9, 0x00, 0x81, 0x10, 0xa2, 0x00, 0xa9, 0x01, 0x81, 0x10, 0x60, 0xa2, 0x00, 0xea,
      0xea, 0xca, 0xd0, 0xfb, 0x60};

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
  bool quit{false};

  // The event data
  SDL_Event e;
  SDL_zero(e);

  BNES::HW::Bus bus;

  SnakeCPU cpu{bus};
  cpu.LoadProgram(program);
  cpu.SetProgramStartAddress(0x600);
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
        }
      }
    }

    try {
      cpu.RunOneInstruction();
    } catch (...) {
      quit = true;
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
