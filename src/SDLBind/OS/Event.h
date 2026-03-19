#ifndef BNES_EVENT_H
#define BNES_EVENT_H

#include "SDLBind/OS/KeyboardEvents.h"
#include "common/Utils.h"

#include <SDL3/SDL_events.h>

#include <variant>

namespace BNES::SDL {
using Event = std::variant<std::monostate, KeyDownEvent, KeyUpEvent>;
ErrorOr<Event> FromSDL(SDL_Event sdl_event);
} // namespace BNES::SDL

#endif
