#ifndef BNES_SDL_WINDOWHANDLE_H
#define BNES_SDL_WINDOWHANDLE_H

#include "SDL/Media.h"
#include "common/Utils.h"

#include <SDL3/SDL.h>

#include <string_view>

namespace BNES::SDL {

class WindowHandle {
public:
  WindowHandle();
  WindowHandle(unsigned int width, unsigned int height);
  WindowHandle(unsigned int width, unsigned int height, std::string_view title);

  ~WindowHandle() { SDL_DestroyWindow(m_window); }

  WindowHandle(const WindowHandle &) = default;
  WindowHandle(WindowHandle &&other) noexcept : m_window(other.m_window) { other.m_window = nullptr; }

  WindowHandle &operator=(const WindowHandle &) = default;
  WindowHandle &operator=(WindowHandle &&other) noexcept {
    m_window = other.m_window;
    other.m_window = nullptr;
    return *this;
  }

  [[nodiscard]] SDL_Window *Handle() const { return m_window; }

  [[nodiscard]] SurfaceHandle Surface() const { return SurfaceHandle{SDL_GetWindowSurface(m_window)}; }

  [[nodiscard]] ErrorOr<void> UpdateSurface() const {
    if (SDL_UpdateWindowSurface(m_window) == false) {
      return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
    }

    return {};
  }

private:
  SDL_Window *m_window{nullptr};

  static constexpr unsigned int DefaultWidth{800};
  static constexpr unsigned int DefaultHeight{600};
};

} // namespace BNES::SDL

#endif
