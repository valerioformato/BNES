//
// Created by Valerio Formato on 30-Jul-25.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDLBind/Buffer.h"

#include <SDL3/SDL.h>

namespace BNES::SDL {

class TextureHandle {
public:
  ~TextureHandle();

  TextureHandle(const TextureHandle &) = default;
  TextureHandle(TextureHandle &&other) noexcept : m_texture(other.m_texture), m_buffer(std::move(other.m_buffer)) {
    other.m_texture = nullptr;
  }
  TextureHandle &operator=(const TextureHandle &) = default;
  TextureHandle &operator=(TextureHandle &&other) noexcept {
    m_texture = other.m_texture;
    m_buffer = std::move(other.m_buffer);
    other.m_texture = nullptr;
    return *this;
  }

  void SetScaleMode(SDL_ScaleMode scale_mode) const { SDL_SetTextureScaleMode(m_texture, scale_mode); }

  void Render(SDL_Renderer *renderer, const SDL_FRect *dest = nullptr);

  [[nodiscard]] SDL_Texture *Get() const { return m_texture; }
  [[nodiscard]] uint32_t Width() const { return m_buffer.Width(); }
  [[nodiscard]] uint32_t Height() const { return m_buffer.Height(); }

  [[nodiscard]] ::BNES::SDL::Buffer &Buffer() { return m_buffer; }
  void Update();

private:
  TextureHandle() = default;

  SDL_Texture *m_texture{nullptr};

  ::BNES::SDL::Buffer m_buffer;

  friend ErrorOr<TextureHandle> MakeTextureFromBuffer(SDL_Renderer *renderer, ::BNES::SDL::Buffer &&buffer);
};

inline ErrorOr<TextureHandle> MakeTextureFromBuffer(SDL_Renderer *renderer, Buffer &&buffer) {
  // Try creating texture with STREAMING access first (more compatible)
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, Pixel::FORMAT, SDL_TEXTUREACCESS_STATIC, buffer.Width(), buffer.Height());

  if (!texture) {
    spdlog::error("Failed to create STATIC texture: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  TextureHandle result;
  result.m_buffer = std::move(buffer);
  result.m_texture = texture;

  return result;
}
} // namespace BNES::SDL

#endif // TEXTURE_H
