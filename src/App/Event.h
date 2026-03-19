#ifndef BNES_APP_EVENT_H
#define BNES_APP_EVENT_H

#include <variant>

namespace BNES {
struct QuitEvent {};
struct StepEvent {};
struct ContinueEvent {};

using Event = std::variant<std::monostate, QuitEvent, StepEvent, ContinueEvent>;
} // namespace BNES

#endif
