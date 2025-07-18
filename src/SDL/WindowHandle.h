#ifndef BNES_SDL_WINDOWHANDLE_H
#define BNES_SDL_WINDOWHANDLE_H

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

  [[nodiscard]] SDL_Surface *Surface() const { return SDL_GetWindowSurface(m_window); }

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
