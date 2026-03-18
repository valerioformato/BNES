#include "App/Application.h"

#include "HW/Constants.h"

namespace BNES {
ErrorOr<void> App::Run() {
  constexpr unsigned int MAIN_WINDOW_W = BNES::HW::NES_SCREEN_W * 6 + 20;
  constexpr unsigned int MAIN_WINDOW_H = BNES::HW::NES_SCREEN_H * 4;
  constexpr double BNES_TARGET_FPS = 60.0;

  TRY(m_bus.LoadRom(m_options.rom_path));
  m_cpu.Init();
  m_ppu.Init();

  // Create the main screen window
  m_main_window = TRY(BNES::SDL::Window::FromSpec({
      .width = MAIN_WINDOW_W,
      .height = MAIN_WINDOW_H,
      .title = "BNES",
      .should_steal_focus = true,
  }));
  m_main_window.Present();

  // setup the m_screen
  TRY(m_screen.Init(m_main_window));

  auto time_point = std::chrono::system_clock::now();

  while (true) {
    bool proceed = false;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
        case SDLK_S:
          if (!m_options.stepping) {
            m_options.stepping = true;
            spdlog::info("Single stepping enabled. Press 's' to step through instructions.");
          } else {
            proceed = true;
          }
          break;
        case SDLK_C:
          if (m_options.stepping) {
            m_options.stepping = false;
            spdlog::info("Single stepping disabled. Execution continues.");
          }
          break;
        case SDLK_Q:
        case SDLK_ESCAPE:
          spdlog::info("Quit requested");
          return {};
        }
        break;
      case SDL_EVENT_QUIT:
        spdlog::info("Quit requested");
        return {};
      }
    }

    // If stepping is enabled and we haven't received a proceed signal, wait for events
    if (m_options.stepping && !proceed) {
      SDL_Delay(10); // Small delay to avoid busy-waiting
      continue;
    }

    // Check if we hit a BRK instruction (opcode 0x00) - stop execution
    uint8_t opcode = m_bus.Read(m_cpu.ProgramCounter());
    if (opcode == 0x00) {
      break;
    }

    constexpr auto frame_cycles = static_cast<size_t>(BNES::HW::NES_CPU_FREQ_HZ / BNES_TARGET_FPS);
    const auto target_cpu_cycles = m_options.stepping ? m_cpu.Cycles() + 1 : m_cpu.Cycles() + frame_cycles;

    auto time_since_last_frame_update = std::chrono::system_clock::now() - time_point;

    if (time_since_last_frame_update > std::chrono::microseconds(16667)) {
      time_point = std::chrono::system_clock::now();

      m_main_window.Clear();

      TRY(m_screen.DrawScreen(m_main_window, 4.0f));

      // Draw CPU debug info
      TRY(m_cpu_debugger.BuildTexture(m_main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPosition(m_main_window.Renderer(), {BNES::HW::NES_SCREEN_W * 4, 0});
      }));

      // Draw PPU debug info
      TRY(m_ppu_debugger.BuildChrRomTexture(m_main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPositionAndScale(m_main_window.Renderer(),
                                            {BNES::HW::NES_SCREEN_W * 4, BNES::HW::NES_SCREEN_H * 3}, 2.0f);
      }));
      TRY(m_ppu_debugger.BuildPPURegisterText(m_main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPosition(m_main_window.Renderer(), {BNES::HW::NES_SCREEN_W * 4, BNES::HW::NES_SCREEN_H * 2});
      }));
      TRY(m_ppu_debugger.BuildPaletteTexture(m_main_window).and_then([&](auto &&tex) {
        return tex.RenderAtPosition(m_main_window.Renderer(), {BNES::HW::NES_SCREEN_W * 5, BNES::HW::NES_SCREEN_H * 2});
      }));

      m_main_window.Present();

      while (m_cpu.Cycles() < target_cpu_cycles) {
        m_cpu.RunInstruction(m_cpu.DecodeNextInstruction());
      }
    }
  }
  return {};
}
} // namespace BNES
