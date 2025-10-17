#include "SDLBind/WindowHandle.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace BNES::SDL {
ErrorOr<TextureHandle> WindowHandle::CreateTexture(Buffer &&buffer) const {
  return std::move(MakeTextureFromBuffer(m_renderer, std::move(buffer)));
}

ErrorOr<WindowHandle> MakeWindow() { return MakeWindow(WindowHandle::WindowSpec{}); }

ErrorOr<WindowHandle> MakeWindow(WindowHandle::WindowSpec spec) {
  // Create window and renderer for this window
  SDL_Window *window_ptr{nullptr};
  SDL_Renderer *renderer_ptr{nullptr};

  if (SDL_CreateWindowAndRenderer(spec.title.data(), spec.width, spec.height, std::to_underlying(spec.flags),
                                  &window_ptr, &renderer_ptr) == false) {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  return WindowHandle{window_ptr, renderer_ptr};
}
} // namespace BNES::SDL
