#include "SDL/WindowHandle.h"

#include <stdexcept>

namespace BNES::SDL {
WindowHandle::WindowHandle() : WindowHandle(DefaultWidth, DefaultHeight) {}

WindowHandle::WindowHandle(unsigned int width, unsigned int height) : WindowHandle(width, height, "BNES SDL Window") {}

WindowHandle::WindowHandle(unsigned int width, unsigned int height, std::string_view title) {
  // Create window
  if (m_window = SDL_CreateWindow("SDL3 Tutorial: Hello SDL3", width, height, 0); m_window == nullptr) {
    // TODO: pass to spdlog for logging
    SDL_Log("Window could not be created! SDL error: %s\n", SDL_GetError());
    throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
  }
}
} // namespace BNES::SDL
