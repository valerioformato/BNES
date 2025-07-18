#include "SDL/Init.h"
#include "SDL/WindowHandle.h"

#include <string>

// The image we will load and show on the screen
SDL_Surface *gHelloWorld{nullptr};

// Screen dimension constants
constexpr int kScreenWidth{640};
constexpr int kScreenHeight{480};

// Starts up SDL and creates window

// Loads media
bool loadMedia() {
  // File loading flag
  bool success{true};

  // Load splash image
  std::string imagePath{"hello-sdl3.bmp"};
  if (gHelloWorld = SDL_LoadBMP(imagePath.c_str()); gHelloWorld == nullptr) {
    SDL_Log("Unable to load image %s! SDL Error: %s\n", imagePath.c_str(), SDL_GetError());
    success = false;
  }

  return success;
}

// Frees media and shuts down SDL
void close() {
  // Clean up surface
  SDL_DestroySurface(gHelloWorld);
  gHelloWorld = nullptr;

  // Quit SDL subsystems
  SDL_Quit();
}

int main(int argc, char **argv) { // Final exit code
  int exitCode{0};

  // Initialize
  if (auto result = BNES::SDL::Init(); !result) {
    SDL_Log("Unable to initialize program!\n");
    exitCode = 1;
  } else {
    auto windowHandle = BNES::SDL::WindowHandle(kScreenWidth, kScreenHeight, "BNES SDL3 Tutorial: Hello SDL3");
    auto screenSurface = windowHandle.Surface();

    // Load media
    if (loadMedia() == false) {
      SDL_Log("Unable to load media!\n");
      exitCode = 2;
    } else {
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
        SDL_FillSurfaceRect(screenSurface, nullptr, SDL_MapSurfaceRGB(screenSurface, 0xFF, 0xFF, 0xFF));

        // Render image on screen
        SDL_BlitSurface(gHelloWorld, nullptr, screenSurface, nullptr);

        // Update the surface
        windowHandle.UpdateSurface();
      }
    }
  }

  // Clean up
  close();

  return exitCode;
}
