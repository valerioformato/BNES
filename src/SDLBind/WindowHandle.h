#ifndef BNES_SDL_WINDOWHANDLE_H
#define BNES_SDL_WINDOWHANDLE_H

#include "SDLBind/Media.h"
#include "SDLBind/TextureHandle.h"
#include "common/Utils.h"

#include <SDL3/SDL.h>

#include <string_view>

namespace BNES::SDL {

enum class WindowFlag {
  None = 0,
  Fullscreen = SDL_WINDOW_FULLSCREEN,
  OpenGL = SDL_WINDOW_OPENGL,
  Hidden = SDL_WINDOW_HIDDEN,
  Borderless = SDL_WINDOW_BORDERLESS,
  Resizable = SDL_WINDOW_RESIZABLE,
  Minimized = SDL_WINDOW_MINIMIZED,
  Maximized = SDL_WINDOW_MAXIMIZED,
};

class WindowHandle {
public:
  WindowHandle() = default;

  ~WindowHandle() {
    SDL_DestroyWindow(m_window);
    SDL_DestroyRenderer(m_renderer);
  }

  WindowHandle(const WindowHandle &) = default;
  WindowHandle(WindowHandle &&other) noexcept : m_window(other.m_window), m_renderer(other.m_renderer) {
    other.m_window = nullptr;
    other.m_renderer = nullptr;
  }

  WindowHandle &operator=(const WindowHandle &) = default;
  WindowHandle &operator=(WindowHandle &&other) noexcept {
    m_window = other.m_window;
    m_renderer = other.m_renderer;
    other.m_window = nullptr;
    other.m_renderer = nullptr;
    return *this;
  }

  [[nodiscard]] SDL_Window *Handle() const { return m_window; }
  [[nodiscard]] SDL_Renderer *Renderer() const { return m_renderer; }

  [[nodiscard]] SurfaceHandle Surface() const { return SurfaceHandle{SDL_GetWindowSurface(m_window)}; }
  [[nodiscard]] ErrorOr<TextureHandle> CreateTexture(Buffer &&buffer) const;

  [[nodiscard]] ErrorOr<void> UpdateSurface() const {
    if (SDL_UpdateWindowSurface(m_window) == false) {
      return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
    }

    return {};
  }

  void SetRenderScale(float scale_x, float scale_y) const { SDL_SetRenderScale(m_renderer, scale_x, scale_y); }

private:
  WindowHandle(SDL_Window *window, SDL_Renderer *renderer) : m_window(window), m_renderer(renderer) {}

  SDL_Window *m_window{nullptr};
  SDL_Renderer *m_renderer{nullptr};

  static constexpr unsigned int DefaultWidth{800};
  static constexpr unsigned int DefaultHeight{600};

public:
  struct WindowSpec {
    unsigned int width{DefaultWidth};
    unsigned int height{DefaultHeight};
    std::string_view title{"BNES Emulator"};
    WindowFlag flags{WindowFlag::None};
  };

  friend ErrorOr<WindowHandle> MakeWindow();
  friend ErrorOr<WindowHandle> MakeWindow(WindowSpec);
};

ErrorOr<WindowHandle> MakeWindow();
ErrorOr<WindowHandle> MakeWindow(WindowHandle::WindowSpec);

} // namespace BNES::SDL

#endif
