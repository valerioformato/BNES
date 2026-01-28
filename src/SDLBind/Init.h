#ifndef BNES_SDL_INIT_H
#define BNES_SDL_INIT_H

#include "SDLBind/Text/Font.h"
#include "common/Utils.h"

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

namespace BNES::SDL {

inline ErrorOr<void> Init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    spdlog::error("SDL could not initialize! SDL error: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

#if defined(SDL_TTF_MAJOR_VERSION)
  if (TTF_Init() == false) {
    spdlog::error("SDL_ttf could not initialize! SDL_ttf error: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }
#endif

  return {};
}

inline void Quit() {
  // Quit SDL subsystems
  ReleaseFonts();
  TTF_Quit();
  SDL_Quit();
}

} // namespace BNES::SDL

#endif
