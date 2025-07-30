//
// Created by Valerio Formato on 30-Jul-25.
//

#include "Buffer.h"

#include <ranges>
#include <span>

namespace BNES::SDL {

Buffer::Buffer(const Buffer &other) : m_data(SDL_CreateSurface(other.m_data->w, other.m_data->h, Pixel::FORMAT)) {
  std::span src_pixels(reinterpret_cast<Pixel *>(other.m_data->pixels), other.m_data->w * other.m_data->h);
  std::span dst_pixels(reinterpret_cast<Pixel *>(m_data->pixels), m_data->w * m_data->h);

  std::ranges::copy(src_pixels, dst_pixels.begin());
}

Buffer &Buffer::operator=(const Buffer &other) {
  *this = Buffer{other}; // Use copy constructor to handle self-assignment
  return *this;
}

ErrorOr<void> Buffer::WritePixel(uint32_t x, uint32_t y, Pixel pixel) {
  auto idx = PixelIndex(x, y);
  if (idx > (m_data->w * m_data->h)) {
    return make_error(std::make_error_code(std::errc::invalid_argument), "Pixel coordinates out of bounds");
  }

  const std::span<Pixel> pixels(reinterpret_cast<Pixel *>(m_data->pixels), m_data->w * m_data->h);
  pixels[idx] = pixel;

  return {};
}

size_t Buffer::PixelIndex(uint32_t x, uint32_t y) const { return (y * m_data->w + x); }

ErrorOr<Buffer> MakeBuffer(uint32_t width, uint32_t height) {
  auto surf_ptr = SDL_CreateSurface(width, height, Pixel::FORMAT);
  if (!surf_ptr) {
    return make_error(std::make_error_code(std::errc::invalid_argument), "Failed to create buffer");
  }

  // Initialize the surface with a default color (black with full alpha)
  // SDL_MapRGBA expects individual color components, not a format pointer
  Uint32 black_color = SDL_MapSurfaceRGBA(surf_ptr, 0, 0, 0, 255);
  if (SDL_FillSurfaceRect(surf_ptr, nullptr, black_color) == false) {
    SDL_DestroySurface(surf_ptr);
    return make_error(std::make_error_code(std::errc::io_error), "Failed to initialize buffer");
  }

  return Buffer{surf_ptr};
}
} // namespace BNES::SDL