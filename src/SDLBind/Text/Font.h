//
// Created by vformato on 12/15/25.
//

#ifndef FONT_H
#define FONT_H

#include "common/Types/Error.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <magic_enum.hpp>

#include <filesystem>
#include <string>
#include <unordered_set>

namespace BNES::SDL {
enum class FontVariant {
  Regular,
  Bold,
  Italic,
  BoldItalic,
};

class Font {
public:
  bool operator==(const Font &other) const { return name == other.name || font == other.font; }

  std::string name;
  TTF_Font *font{nullptr};

  static ErrorOr<Font> FromFile(std::string name, FontVariant variant);
};
} // namespace BNES::SDL

template <> struct std::hash<BNES::SDL::Font> {
  std::size_t operator()(const BNES::SDL::Font &h) const noexcept { return std::hash<std::string>{}(h.name); }
};

#endif // FONT_H
