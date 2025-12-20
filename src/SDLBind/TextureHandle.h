//
// Created by Valerio Formato on 30-Jul-25.
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include "SDLBind/Buffer.h"
#include "SDLBind/Font.h"

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
  friend ErrorOr<TextureHandle> MakeTextureFromText(SDL_Renderer *renderer, std::string_view content,
                                                    const FontHandle &font, SDL_Color color);
  friend ErrorOr<TextureHandle> MakeTextureFromTextWrapped(SDL_Renderer *renderer, std::string_view content,
                                                           const FontHandle &font, SDL_Color color, unsigned int width);
};

ErrorOr<TextureHandle> MakeTextureFromBuffer(SDL_Renderer *renderer, Buffer &&buffer);
#if defined(SDL_TTF_MAJOR_VERSION)
ErrorOr<TextureHandle> MakeTextureFromText(SDL_Renderer *renderer, std::string_view content, const FontHandle &font,
                                           SDL_Color color);
ErrorOr<TextureHandle> MakeTextureFromTextWrapped(SDL_Renderer *renderer, std::string_view content,
                                                  const FontHandle &font, SDL_Color color, unsigned int width);
#endif
} // namespace BNES::SDL

#endif // TEXTURE_H
