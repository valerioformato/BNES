//
// Created by Valerio Formato on 30-Jul-25.
//

#ifndef BUFFER_H
#define BUFFER_H

#include "SDL/Media.h"

#include <vector>

namespace BNES::SDL {
struct Pixel {
  static constexpr auto FORMAT = SDL_PIXELFORMAT_RGBA8888;

  uint8_t r{0};
  uint8_t g{0};
  uint8_t b{0};
  uint8_t a{255}; // Default alpha to fully opaque
};

class Buffer {
  friend ErrorOr<Buffer> MakeBuffer(uint32_t width, uint32_t height);

public:
  Buffer() = default;
  ~Buffer() { SDL_DestroySurface(m_data); }

  Buffer(const Buffer &);
  Buffer(Buffer &&other) noexcept : m_data(other.m_data) { other.m_data = nullptr; }
  Buffer &operator=(const Buffer &);
  Buffer &operator=(Buffer &&other) noexcept {
    m_data = other.m_data;
    other.m_data = nullptr;
    return *this;
  }

  [[nodiscard]] uint32_t Width() const { return m_data->w; }
  [[nodiscard]] uint32_t Height() const { return m_data->h; }

  SurfaceHandle AsSurface() { return SurfaceHandle{m_data, true}; };

  ErrorOr<void> WritePixel(uint32_t x, uint32_t y, Pixel pixel);

private:
  explicit Buffer(SDL_Surface *data) : m_data{data} {}

  size_t PixelIndex(uint32_t x, uint32_t y) const;

  SDL_Surface *m_data{nullptr};
};

ErrorOr<Buffer> MakeBuffer(uint32_t width, uint32_t height);

} // namespace BNES::SDL

#endif // BUFFER_H
