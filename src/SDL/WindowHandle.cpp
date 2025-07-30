#include "SDL/WindowHandle.h"

#include <spdlog/spdlog.h>

#include <stdexcept>

namespace BNES::SDL {
ErrorOr<TextureHandle> WindowHandle::CreateTexture(Buffer &&buffer) const {
  return std::move(MakeTextureFromBuffer(m_renderer, std::move(buffer)));
}

ErrorOr<WindowHandle> MakeWindow() { return MakeWindow(WindowHandle::DefaultHeight, WindowHandle::DefaultWidth); }

ErrorOr<WindowHandle> MakeWindow(unsigned int width, unsigned int height) {
  return MakeWindow(width, height, "BNES emulator");
}

ErrorOr<WindowHandle> MakeWindow(unsigned int width, unsigned int height, std::string_view title) {
  // Create window and renderer for this window
  SDL_Window *window_ptr{nullptr};
  SDL_Renderer *renderer_ptr{nullptr};

  if (SDL_CreateWindowAndRenderer(title.data(), width, height, 0, &window_ptr, &renderer_ptr) == false) {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  return WindowHandle{window_ptr, renderer_ptr};
}
} // namespace BNES::SDL
