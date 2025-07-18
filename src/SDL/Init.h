#ifndef BNES_SDL_INIT_H
#define BNES_SDL_INIT_H

#include "common/Utils.h"

#include <SDL3/SDL.h>

#include <utility>

namespace BNES::SDL {
inline ErrorOr<void> Init() {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) == false) {
    SDL_Log("SDL could not initialize! SDL error: %s\n", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  return {};
}
} // namespace BNES::SDL

#endif
