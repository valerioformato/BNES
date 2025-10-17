//
// Created by vformato on 10/17/25.
//

#include "CPUDebugger.h"

namespace BNES::Tools {

CPUDebugger::Window::Window(SDL::Buffer &&buffer)
    : m_window_handle(SDL::MakeWindow(SDL::WindowHandle::WindowSpec{
                                          .width = buffer.Width(),
                                          .height = buffer.Height(),
                                          .title = "CPU Debugger",
                                          .flags = SDL::WindowFlag::None,
                                      })
                          .value()),
      m_texture(m_window_handle.CreateTexture(std::move(buffer)).value()) {}

void CPUDebugger::Window::Update() {
  SDL_RenderClear(m_window_handle.Renderer());

  SDL_RenderPresent(m_window_handle.Renderer());
}

void CPUDebugger::Update() { m_window.Update(); }

} // namespace BNES::Tools
