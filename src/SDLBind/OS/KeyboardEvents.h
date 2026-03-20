#ifndef BNES_KEYBOARDEVENTS_H
#define BNES_KEYBOARDEVENTS_H

#include <SDL3/SDL_keycode.h>
#include <magic_enum.hpp>

namespace BNES::SDL {

enum class KeyBoardKey : unsigned int {
  // Arrow keys
  ArrowUp = SDLK_UP,
  ArrowDown = SDLK_DOWN,
  ArrowLeft = SDLK_LEFT,
  ArrowRight = SDLK_RIGHT,
  // Letter keys
  A = SDLK_A,
  B = SDLK_B,
  C = SDLK_C,
  D = SDLK_D,
  E = SDLK_E,
  F = SDLK_F,
  G = SDLK_G,
  H = SDLK_H,
  I = SDLK_I,
  J = SDLK_J,
  K = SDLK_K,
  L = SDLK_L,
  M = SDLK_M,
  N = SDLK_N,
  O = SDLK_O,
  P = SDLK_P,
  Q = SDLK_Q,
  R = SDLK_R,
  S = SDLK_S,
  T = SDLK_T,
  U = SDLK_U,
  V = SDLK_V,
  W = SDLK_W,
  X = SDLK_X,
  Y = SDLK_Y,
  Z = SDLK_Z,
  // misc keys
  Plus = SDLK_PLUS,
  Comma = SDLK_COMMA,
  Minus = SDLK_MINUS,
  Period = SDLK_PERIOD,
  Slash = SDLK_SLASH,
  Escape = SDLK_ESCAPE,
  Space = SDLK_SPACE,
  Return = SDLK_RETURN,
  // TODO: maybe add the rest
};

struct KeyDownEvent {
  KeyBoardKey key;
};

struct KeyUpEvent {
  KeyBoardKey key;
};
} // namespace BNES::SDL
#endif
