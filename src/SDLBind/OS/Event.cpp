#include "SDLBind/OS/Event.h"
#include "SDL3/SDL_events.h"
#include "SDLBind/OS/KeyboardEvents.h"

#include <magic_enum.hpp>

namespace BNES::SDL {
ErrorOr<Event> FromSDL(SDL_Event sdl_event) {
  switch (sdl_event.type) {
  case SDL_EVENT_KEY_DOWN:
    return KeyDownEvent{.key = static_cast<KeyBoardKey>(sdl_event.key.key)};
  case SDL_EVENT_KEY_UP:
    return KeyUpEvent{.key = static_cast<KeyBoardKey>(sdl_event.key.key)};
  }

  spdlog::trace("SDL event not implemented yet: 0x{:X}", sdl_event.type);
  return std::monostate{};
}
} // namespace BNES::SDL
