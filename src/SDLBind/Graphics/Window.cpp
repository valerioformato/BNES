#include "SDLBind/Graphics/Window.h"

#include <spdlog/spdlog.h>

namespace BNES::SDL {

Window::~Window() {
  SDL_DestroyWindow(m_window);
  SDL_DestroyRenderer(m_renderer);
}

ErrorOr<Texture> Window::CreateTexture(Buffer &&buffer) const {
  return Texture::FromBuffer(m_renderer, std::move(buffer));
}

std::array<int, 2> Window::Position() const {
  std::array<int, 2> position;
  SDL_GetWindowPosition(m_window, &position[0], &position[1]);

  return position;
}

std::array<int, 2> Window::Size() const {
  std::array<int, 2> size;
  SDL_GetWindowSize(m_window, &size[0], &size[1]);

  return size;
}

ErrorOr<void> Window::SetPosition(unsigned int x, unsigned int y) const {
  if (SDL_SetWindowPosition(m_window, x, y) == false) {
    return make_error(std::errc::not_supported, SDL_GetError());
  }

  return {};
}

ErrorOr<Window> Window::CreateDefault() { return FromSpec(WindowSpec{}); }

ErrorOr<Window> Window::FromSpec(WindowSpec spec) {
  // Create window and renderer for this window
  SDL_Window *window_ptr{nullptr};
  SDL_Renderer *renderer_ptr{nullptr};

  if (SDL_CreateWindowAndRenderer(spec.title.data(), spec.width, spec.height, std::to_underlying(spec.flags),
                                  &window_ptr, &renderer_ptr) == false) {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  return Window{window_ptr, renderer_ptr};
}
} // namespace BNES::SDL
