//
// Created by vformato on 12/22/25.
//

#ifndef COLOR_H
#define COLOR_H

#include <SDL3/SDL.h>

#include <string>

namespace BNES::SDL {
struct Color {
  uint8_t r{0};
  uint8_t g{0};
  uint8_t b{0};
  uint8_t a{255}; // Default alpha to fully opaque

  SDL_Color ToSDL_Color() { return SDL_Color{r, g, b, a}; }

  auto operator<=>(const Color &) const = default;
};

struct Pixel {
  static constexpr auto FORMAT = SDL_PIXELFORMAT_ABGR8888;

  Color color;

  auto operator<=>(const Pixel &) const = default;
};

std::string format_as(Pixel pixel);

} // namespace BNES::SDL
#endif // COLOR_H
