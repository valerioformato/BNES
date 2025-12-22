//
// Created by Valerio Formato on 30-Jul-25.
//

#include "SDLBind/Graphics/Buffer.h"

namespace BNES::SDL {

std::string format_as(Pixel pixel) {
  return fmt::format("[R: {}, G: {}, B: {}, A: {}]", pixel.color.r, pixel.color.g, pixel.color.b, pixel.color.a);
}

Buffer::Buffer(const Buffer &other) : m_data{other.m_data} { RePointPixels(); }
Buffer &Buffer::operator=(const Buffer &other) {
  *this = Buffer{other}; // Use copy constructor to handle self-assignment
  return *this;
}

ErrorOr<void> Buffer::WritePixel(uint32_t x, uint32_t y, Pixel pixel) {
  uint32_t idx = PixelIndex(x, y);
  if (idx > uint32_t(m_data.SDLType()->w * m_data.SDLType()->h)) {
    return make_error(std::make_error_code(std::errc::invalid_argument), "Pixel coordinates out of bounds");
  }

  m_pixels[idx] = pixel;

  return {};
}

ErrorOr<void> Buffer::WritePixel(uint32_t idx, Pixel pixel) {
  if (idx > uint32_t(m_data.SDLType()->w * m_data.SDLType()->h)) {
    return make_error(std::make_error_code(std::errc::invalid_argument), "Pixel coordinates out of bounds");
  }

  m_pixels[idx] = pixel;

  return {};
}

size_t Buffer::PixelIndex(uint32_t x, uint32_t y) const { return (y * m_data.SDLType()->w + x); }

void Buffer::RePointPixels() {
  m_pixels = std::span<Pixel>(reinterpret_cast<Pixel *>(m_data.SDLType()->pixels),
                              static_cast<size_t>(m_data.SDLType()->w * m_data.SDLType()->h));
}

ErrorOr<Buffer> Buffer::FromSize(uint32_t width, uint32_t height) {
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

namespace fs = std::filesystem;

ErrorOr<Buffer> Buffer::FromBMP(fs::path path) {
  if (fs::exists(path) == false) {
    return make_error(std::make_error_code(std::errc::no_such_file_or_directory),
                      "File does not exist: " + path.string());
  }

  if (auto surface = SDL_LoadBMP(path.string().c_str()); surface != nullptr) {
    return Buffer{surface};
  } else {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }
}
} // namespace BNES::SDL
