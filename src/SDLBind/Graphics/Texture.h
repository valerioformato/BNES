//
// Created by Valerio Formato on 30-Jul-25.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDLBind/Graphics/Buffer.h"
#include "SDLBind/Text/Font.h"
#include "SDLBind/Text/TextSpec.h"

#include <SDL3/SDL.h>

namespace BNES::SDL {

class Texture {
public:
  // Static factory functions
  static ErrorOr<Texture> FromBuffer(SDL_Renderer *renderer, Buffer &&buffer);
#if defined(SDL_TTF_MAJOR_VERSION)
  static ErrorOr<Texture> FromText(SDL_Renderer *renderer, TextSpec text);
#endif

  ~Texture();

  Texture(const Texture &) = delete;
  Texture(Texture &&other) noexcept : m_texture(other.m_texture), m_buffer(std::move(other.m_buffer)) {
    other.m_texture = nullptr;
  }
  Texture &operator=(const Texture &) = delete;
  Texture &operator=(Texture &&other) noexcept {
    m_texture = other.m_texture;
    m_buffer = std::move(other.m_buffer);
    other.m_texture = nullptr;
    return *this;
  }

  void SetScaleMode(SDL_ScaleMode scale_mode) const { SDL_SetTextureScaleMode(m_texture, scale_mode); }

  void Render(SDL_Renderer *renderer, const SDL_FRect *dest = nullptr);

  [[nodiscard]] uint32_t Width() const { return m_buffer.Width(); }
  [[nodiscard]] uint32_t Height() const { return m_buffer.Height(); }

  [[nodiscard]] ::BNES::SDL::Buffer &Buffer() { return m_buffer; }
  [[nodiscard]] ErrorOr<void> Update();

private:
  Texture() = default;
  [[nodiscard]] SDL_Texture *SDLType() const { return m_texture; }

  SDL_Texture *m_texture{nullptr};

  ::BNES::SDL::Buffer m_buffer;
};
} // namespace BNES::SDL

#endif // TEXTURE_H
