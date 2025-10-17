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
  } else {
    spdlog::debug("Texture update successful");
  }
}

} // namespace BNES::SDL
