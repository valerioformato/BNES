#ifndef BNES_SDL_MEDIA_H
#define BNES_SDL_MEDIA_H

#include "common/Utils.h"

#include <SDL3/SDL.h>

#include <filesystem>

namespace BNES::SDL {

class SurfaceHandle {
public:
  explicit SurfaceHandle(SDL_Surface *surface) : m_surface(surface) {}
  ~SurfaceHandle() { SDL_DestroySurface(m_surface); }
  SurfaceHandle(const SurfaceHandle &) = default;
  SurfaceHandle(SurfaceHandle &&other) noexcept : m_surface(other.m_surface) { other.m_surface = nullptr; }

  [[nodiscard]] SDL_Surface *Handle() const { return m_surface; }

private:
  SDL_Surface *m_surface{nullptr};
};

namespace Media {

[[nodiscard]] ErrorOr<SurfaceHandle> LoadBMP(std::filesystem::path path);

} // namespace Media
} // namespace BNES::SDL
#endif
