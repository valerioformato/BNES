//
// Created by Valerio Formato on 30-Jul-25.
//

#include "SDLBind/Graphics/Texture.h"

#include <spdlog/spdlog.h>

namespace BNES::SDL {
Texture::~Texture() { SDL_DestroyTexture(m_texture); }

void Texture::Render(SDL_Renderer *renderer, const SDL_FRect *dest) {
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

ErrorOr<void> Texture::Update() {
  // For regular textures, we need to update from the surface
  SDL_Surface *surface = m_buffer.Surface().SDLType();

  if (SDL_UpdateTexture(m_texture, nullptr, surface->pixels, surface->pitch) == false) {
    return make_error(std::make_error_code(std::errc::invalid_argument), SDL_GetError());
  }

  return {};
}

ErrorOr<Texture> Texture::FromBuffer(SDL_Renderer *renderer, ::BNES::SDL::Buffer &&buffer) {
  // Try creating texture with STREAMING access first (more compatible)
  SDL_Texture *texture =
      SDL_CreateTexture(renderer, Pixel::FORMAT, SDL_TEXTUREACCESS_STATIC, buffer.Width(), buffer.Height());

  if (!texture) {
    spdlog::error("Failed to create STATIC texture: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  Texture result;
  result.m_buffer = std::move(buffer);
  result.m_texture = texture;

  return result;
}

#if defined(SDL_TTF_MAJOR_VERSION)
ErrorOr<Texture> Texture::FromText(SDL_Renderer *renderer, TextSpec spec) {
  SDL_Surface *surface;
  auto &[content, font, color, wrapping, width] = spec;

  if (surface =
          wrapping == TextWrapping::Wrapped
              ? TTF_RenderText_Blended_Wrapped(font.font, content.data(), content.size(), color.ToSDL_Color(), width)
              : TTF_RenderText_Blended(font.font, content.data(), content.size(), color.ToSDL_Color());
      surface == nullptr) {
    spdlog::error("Failed to render text: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  SDL_Texture *texture;
  if (texture = SDL_CreateTextureFromSurface(renderer, surface); texture == nullptr) {
    spdlog::error("Failed to create texture: {}", SDL_GetError());
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  Texture result;
  result.m_buffer = ::BNES::SDL::Buffer{surface};
  result.m_texture = texture;

  return result;
}

#endif

} // namespace BNES::SDL
