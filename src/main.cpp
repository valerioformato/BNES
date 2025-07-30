#include "SDL/Buffer.h"
#include "SDL/Init.h"
#include "SDL/WindowHandle.h"

#include <string>

// Frees media and shuts down SDL
void close() {
  // Quit SDL subsystems
  SDL_Quit();
}

namespace fs = std::filesystem;

int main(int argc, char **argv) { // Final exit code

  auto clock = std::chrono::high_resolution_clock{};
  auto frame_duration = std::chrono::duration<double>(1.0f / 60.0f);

  // Initialize SDL
  if (auto result = BNES::SDL::Init(); !result) {
    spdlog::error("Unable to initialize program!");
    return 1;
  }

  auto window_handle = BNES::SDL::MakeWindow(640, 480).value();
  auto screen_surface = window_handle.Surface();

  auto buffer = BNES::SDL::MakeBuffer(640, 480).value();
  auto maybe_texture = window_handle.CreateTexture(std::move(buffer));
  if (!maybe_texture) {
    spdlog::error("Unable to create texture: {}", maybe_texture.error().Message());
    return 1;
  }
  auto &texture = maybe_texture.value();

  // The quit flag
  bool quit{false};

  // The event data
  SDL_Event e;
  SDL_zero(e);

  unsigned int x{0};
  unsigned int y{0};

  // The main loop
  while (quit == false) {
    auto begin = clock.now();

    // Get event data
    while (SDL_PollEvent(&e) == true) {
      // If event is quit type
      if (e.type == SDL_EVENT_QUIT) {
        // End the main loop
        quit = true;
      }
    }

    texture.Buffer().WritePixel(x, y, BNES::SDL::Pixel{255, 0, 0, 255}); // Write a red pixel
    ++x;
    if (x >= texture.Buffer().Width()) {
      x = 0;
      ++y;
    }
    texture.Update();

    // Set a blue background to distinguish from black texture
    SDL_SetRenderDrawColor(window_handle.Renderer(), 0, 0, 255, 255);
    SDL_RenderClear(window_handle.Renderer());

    texture.Render(window_handle.Renderer());

    SDL_RenderPresent(window_handle.Renderer());

    // lock 60 fps for now
    std::this_thread::sleep_for(frame_duration - (clock.now() - begin));
  }

  // Clean up
  BNES::SDL::Quit();

  return 0;
}
