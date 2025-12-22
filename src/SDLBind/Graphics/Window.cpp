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
