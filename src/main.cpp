#include "SDL/Init.h"
#include "SDL/Media.h"
#include "SDL/WindowHandle.h"

#include <string>

// Frees media and shuts down SDL
void close() {
  // Quit SDL subsystems
  SDL_Quit();
}

namespace fs = std::filesystem;

int main(int argc, char **argv) { // Final exit code

  // Initialize SDL
  if (auto result = BNES::SDL::Init(); !result) {
    SDL_Log("Unable to initialize program!\n");
    return 1;
  }

  auto window_handle = BNES::SDL::WindowHandle(640, 480, "BNES SDL3 Tutorial: Hello SDL3");
  auto screen_surface = window_handle.Surface();

  fs::path image_path{"hello-sdl3.bmp"};

  auto hello_world_result = BNES::SDL::Media::LoadBMP(image_path);
  if (!hello_world_result) {
    SDL_Log("Unable to load image %s! SDL Error: %s\n", image_path.string().c_str(),
            hello_world_result.error().Message().data());
    return 2;
  }
  auto hello_world = hello_world_result.value();

  // The quit flag
  bool quit{false};

  // The event data
  SDL_Event e;
  SDL_zero(e);

  // The main loop
  while (quit == false) {
    // Get event data
    while (SDL_PollEvent(&e) == true) {
      // If event is quit type
      if (e.type == SDL_EVENT_QUIT) {
        // End the main loop
        quit = true;
      }
    }

    // Fill the surface white
    SDL_FillSurfaceRect(screen_surface.Handle(), nullptr, SDL_MapSurfaceRGB(screen_surface.Handle(), 0xFF, 0xFF, 0xFF));

    // Render image on screen
    SDL_BlitSurface(hello_world.Handle(), nullptr, screen_surface.Handle(), nullptr);

    // Update the surface
    window_handle.UpdateSurface();
  }

  // Clean up
  BNES::SDL::Quit();

  return 0;
}
