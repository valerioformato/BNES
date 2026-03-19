#include "SDLBind/OS/Event.h"
#include "SDL3/SDL_events.h"
#include "SDLBind/OS/KeyboardEvents.h"

#include <magic_enum.hpp>

namespace BNES::SDL {
ErrorOr<Event> FromSDL(SDL_Event sdl_event) {
  switch (sdl_event.type) {
  case SDL_EVENT_KEY_DOWN:
    if (auto maybe_key = magic_enum::enum_cast<KeyBoardKey>(sdl_event.key.key); maybe_key)
      return KeyDownEvent{.key = maybe_key.value()};
    return make_error(std::errc::argument_out_of_domain, "Key not supported yet");
  case SDL_EVENT_KEY_UP:
    if (auto maybe_key = magic_enum::enum_cast<KeyBoardKey>(sdl_event.key.key); maybe_key)
      return KeyUpEvent{.key = maybe_key.value()};
    return make_error(std::errc::argument_out_of_domain, "Key not supported yet");
  }

  spdlog::trace("SDL event not implemented yet: 0x{:X}", sdl_event.type);
  return std::monostate{};
}
} // namespace BNES::SDL
