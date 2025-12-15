//
// Created by Valerio Formato on 30-Jul-25.
//

#include "TextureHandle.h"

#include <span>
#include <spdlog/spdlog.h>

namespace BNES::SDL {
TextureHandle::~TextureHandle() { SDL_DestroyTexture(m_texture); }

void TextureHandle::Render(SDL_Renderer *renderer, const SDL_FRect *dest) {
  // Check if texture is valid before rendering
  if (!m_texture) {
    spdlog::error("Attempting to render null texture");
    return;
  }

  // Use provided destination or default to full texture size
  SDL_FRect dstRect;
  if (dest) {
    dstRect = *dest;
  } else {
    dstRect = {0.0f, 0.0f, static_cast<float>(m_buffer.Width()), static_cast<float>(m_buffer.Height())};
  }

  // Render texture
  if (SDL_RenderTexture(renderer, m_texture, nullptr, &dstRect) == false) {
    spdlog::error("Failed to render texture: {}", SDL_GetError());
  }
}

void TextureHandle::Update() {
  // For regular textures, we need to update from the surface
  SDL_Surface *surface = m_buffer.AsSurface().Handle();

  if (SDL_UpdateTexture(m_texture, nullptr, surface->pixels, surface->pitch) == false) {
    spdlog::error("Failed to update texture: {}", SDL_GetError());
  }
}

ErrorOr<TextureHandle> MakeTextureFromBuffer(SDL_Renderer *renderer, Buffer &&buffer) {
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

#if defined(SDL_TTF_MAJOR_VERSION)
ErrorOr<TextureHandle> MakeTextureFromText(std::string_view content, FontHandle font, SDL_Color color) {}

ErrorOr<TextureHandle> MakeTextureFromTextWrapped(std::string_view content, FontHandle font, SDL_Color color,
                                                  unsigned int width) {
  SDL_Surface *surface;

  if (surface = TTF_RenderText_Blended_Wrapped(font.font, content.data(), content.size(), color, width);
      surface == nullptr) {
    spdlog::error("Failed to render text: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  // SDL_Texture *texture;
  // if (texture = SDL_CreateTextureFromSurface(renderer, surface); texture == nullptr) {
  //   spdlog::error("Failed to create texture: {}", SDL_GetError());
  //   return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  // }
}
#endif

} // namespace BNES::SDL
