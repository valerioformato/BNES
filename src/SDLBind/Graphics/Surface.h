//
// Created by vformato on 12/22/25.
//

#ifndef SURFACEHANDLE_H
#define SURFACEHANDLE_H

#include "common/Utils.h"

#include <SDL3/SDL.h>

namespace BNES::SDL {

class Surface {
  friend class Buffer;
  friend class Texture;
  friend class Window;

public:
  ~Surface();

  Surface(const Surface &);
  Surface(Surface &&other) noexcept : m_surface(other.m_surface) { other.m_surface = nullptr; }

  Surface &operator=(const Surface &);
  Surface &operator=(Surface &&other) noexcept {
    m_surface = std::move(other.m_surface);
    other.m_surface = nullptr;
    return *this;
  }

  [[nodiscard]] uint32_t Width() const { return m_surface->w; }
  [[nodiscard]] uint32_t Height() const { return m_surface->h; }

  [[nodiscard]] ErrorOr<void> BlitToSurface(Surface &destination) const;

  [[nodiscard]] SDL_Surface *SDLType() const { return m_surface; }

private:
  Surface() = default;
  explicit Surface(SDL_Surface *surface) : m_surface(surface) {}

  SDL_Surface *m_surface{nullptr};
};

} // namespace BNES::SDL

#endif // SURFACEHANDLE_H
