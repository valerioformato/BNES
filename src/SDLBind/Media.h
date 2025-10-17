#ifndef BNES_SDL_MEDIA_H
#define BNES_SDL_MEDIA_H

#include "common/Utils.h"

#include <SDL3/SDL.h>

#include <filesystem>

namespace BNES::SDL {

class SurfaceHandle {
public:
  explicit SurfaceHandle(SDL_Surface *surface) : m_surface(surface) {}
  SurfaceHandle(SDL_Surface *surface, bool is_managed) : m_surface(surface), m_is_managed{is_managed} {}
  ~SurfaceHandle();

  SurfaceHandle(const SurfaceHandle &) = default;
  SurfaceHandle(SurfaceHandle &&other) noexcept : m_surface(other.m_surface) { other.m_surface = nullptr; }

  SurfaceHandle &operator=(const SurfaceHandle &) = default;
  SurfaceHandle &operator=(SurfaceHandle &&other) noexcept {
    other.m_surface = nullptr;
    return *this;
  }

  [[nodiscard]] SDL_Surface *Handle() const { return m_surface; }

  [[nodiscard]] ErrorOr<void> BlitToSurface(SurfaceHandle &destination) const;

private:
  SDL_Surface *m_surface{nullptr};
  bool m_is_managed{false};
};

namespace Media {

[[nodiscard]] ErrorOr<SurfaceHandle> LoadBMP(std::filesystem::path path);

} // namespace Media
} // namespace BNES::SDL
#endif
