//
// Created by Valerio Formato on 09-Mar-26.
//

#ifndef BNES_RENDERTARGET_H
#define BNES_RENDERTARGET_H

#include "SDLBind/Graphics/Texture.h"
#include "common/Types/non_owning_ptr.h"

namespace BNES::SDL {
class ActiveRenderTarget {
public:
  ActiveRenderTarget() = delete;
  ActiveRenderTarget(SDL_Renderer *renderer, const Texture &texture) : m_renderer(renderer) {
    SDL_SetRenderTarget(m_renderer, texture.SDLType());
  }

  ~ActiveRenderTarget() { SDL_SetRenderTarget(m_renderer, nullptr); }

private:
  SDL_Renderer *m_renderer{nullptr};
};
} // namespace BNES::SDL

#endif // BNES_RENDERTARGET_H
