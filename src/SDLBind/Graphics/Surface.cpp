//
// Created by vformato on 12/22/25.
//

#include "SDLBind/Graphics/Surface.h"
#include "SDLBind/Graphics/Color.h"

namespace BNES ::SDL {

Surface::~Surface() { SDL_DestroySurface(m_surface); }

Surface::Surface(const Surface &other) {
  m_surface = SDL_CreateSurface(other.m_surface->w, other.m_surface->h, Pixel::FORMAT);

  std::span src_pixels(reinterpret_cast<Pixel *>(other.m_surface->pixels), other.m_surface->w * other.m_surface->h);
  std::span dst_pixels(reinterpret_cast<Pixel *>(m_surface->pixels), m_surface->w * m_surface->h);

  std::ranges::copy(src_pixels, dst_pixels.begin());
  auto [r1, r2] = std::ranges::mismatch(src_pixels, dst_pixels, std::equal_to<Pixel>());
  assert(r1 == src_pixels.end() || r2 == dst_pixels.end());
}

Surface &Surface::operator=(const Surface &other) {
  *this = Surface{other}; // Use copy constructor to handle self-assignment
  return *this;
}

ErrorOr<void> Surface::BlitToSurface(Surface &destination) const {
  bool result = SDL_BlitSurface(SDLType(), nullptr, destination.SDLType(), nullptr);

  if (result == false) {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  return {};
}
} // namespace BNES::SDL
