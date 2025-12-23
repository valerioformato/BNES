//
// Created by vformato on 9/16/25.
//

#include "../../src/SDLBind/Graphics/Buffer.h"
#include "../../src/SDLBind/Graphics/Window.h"
#include "../../src/SDLBind/Text/Font.h"
#include "HW/CPU.h"
#include "SDLBind/Init.h"
#include "Tools/CPUDebugger.h"
#include "common/Time.h"

#include <algorithm>
#include <iostream>

BNES::ErrorOr<int> test_main() {
  auto clock = std::chrono::high_resolution_clock{};
  static constexpr auto frame_duration = std::chrono::duration<double>(1.0f / 60.0f);

  // Initialize SDL
  if (auto result = BNES::SDL::Init(); !result) {
    spdlog::error("Unable to initialize program!");
    return 1;
  }

  auto window_handle =
      TRY(BNES::SDL::Window::FromSpec({.width = 1024, .height = 768, .title = "Text rendering tests"}));
  auto screen_surface = window_handle.Surface();

  auto font = TRY(BNES::SDL::Font::Get("SpaceMono", BNES::SDL::FontVariant::Regular));
  auto texture =
      TRY(BNES::SDL::Texture::FromText(window_handle.Renderer(), {
                                                                     .content = "Testing testing",
                                                                     .font = font,
                                                                     .color = BNES::SDL::Color{255, 255, 255, 255},
                                                                     .wrapping = BNES::SDL::TextWrapping::Wrapped,
                                                                     .wrap_size = 1024,
                                                                 }));

  fmt::println("texture: {} x {}", texture.Buffer().Width(), texture.Buffer().Height());

  // The quit flag
  bool quit{false};

  // The event data
  SDL_Event e;
  SDL_zero(e);

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
    }

    if (begin - last_frame_update_time > frame_duration) {
      TRY(texture.Update());

      // SDL_RenderClear(window_handle.Renderer());

      TRY(texture.RenderAtPosition(window_handle.Renderer(), {100, 100}));

      SDL_RenderPresent(window_handle.Renderer());
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
  auto result = test_main();
  if (!result) {
    spdlog::error("Error: {}", result.error().Message());
    return result.error().Code().value();
  }

  return 0;
}
