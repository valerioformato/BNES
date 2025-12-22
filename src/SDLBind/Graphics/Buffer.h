//
// Created by Valerio Formato on 30-Jul-25.
//

#ifndef BUFFER_H
#define BUFFER_H

#include "SDLBind/Graphics/Color.h"
#include "SDLBind/Graphics/Surface.h"
#include "common/Utils.h"

#include <filesystem>
#include <span>

namespace BNES::SDL {

class Buffer {
  friend class Texture;

public:
  // Static factory functions
  [[nodiscard]] static ErrorOr<Buffer> FromSize(uint32_t width, uint32_t height);
  [[nodiscard]] static ErrorOr<Buffer> FromBMP(std::filesystem::path path);

  ~Buffer() = default;

  Buffer(const Buffer &);
  Buffer(Buffer &&other) noexcept {
    m_data = std::move(other.m_data);
    RePointPixels();
    other.m_data = {};
    other.m_pixels = {};
  }

  Buffer &operator=(const Buffer &);
  Buffer &operator=(Buffer &&other) noexcept {
    m_data = std::move(other.m_data);
    RePointPixels();
    other.m_data = {};
    other.m_pixels = {};
    return *this;
  }

  [[nodiscard]] uint32_t Width() const { return m_data.Width(); }
  [[nodiscard]] uint32_t Height() const { return m_data.Height(); }

  ::BNES::SDL::Surface &Surface() { return m_data; };
  const ::BNES::SDL::Surface &Surface() const { return m_data; };

  ErrorOr<void> WritePixel(uint32_t x, uint32_t y, Pixel pixel);
  ErrorOr<void> WritePixel(uint32_t index, Pixel pixel);

  [[nodiscard]] std::span<const Pixel> Pixels() const { return m_pixels; }
  [[nodiscard]] std::span<Pixel> Pixels() { return m_pixels; }

private:
  Buffer() = default;
  explicit Buffer(SDL_Surface *surf) : m_data{surf} { RePointPixels(); }

  size_t PixelIndex(uint32_t x, uint32_t y) const;

  void RePointPixels();

  ::BNES::SDL::Surface m_data{nullptr};
  std::span<Pixel> m_pixels;
};
} // namespace BNES::SDL

#endif // BUFFER_H
