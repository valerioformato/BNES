//
// Created by vformato on 12/22/25.
//

#ifndef TEXTSPEC_H
#define TEXTSPEC_H

#include "SDLBind/Text/Font.h"

#include <string_view>
namespace BNES::SDL {
enum class TextWrapping {
  None,
  Wrapped,
};

struct TextSpec {
  std::string_view content;
  Font font;
  Color color;
  TextWrapping wrapping{TextWrapping::None};
  unsigned int wrap_size = 0;
};
} // namespace BNES::SDL

#endif // TEXTSPEC_H
