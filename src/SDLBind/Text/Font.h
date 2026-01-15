//
// Created by vformato on 12/15/25.
//

#ifndef FONT_H
#define FONT_H

#include "common/Types/Error.h"

#include <SDL3_ttf/SDL_ttf.h>
#include <magic_enum.hpp>
#include <spdlog/spdlog.h>

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
  FontVariant variant;
  TTF_Font *font{nullptr};

  static ErrorOr<Font> Get(std::string name, FontVariant variant);

  [[nodiscard]] unsigned int LineSkip() const { return TTF_GetFontLineSkip(font); }

private:
  static size_t Hash(std::string_view name, FontVariant variant) {
    return std::hash<std::string>{}(fmt::format("{}-{}", name, magic_enum::enum_name(variant)));
  };
  static ErrorOr<Font> FromFile(std::string_view name, FontVariant variant);

  friend struct std::hash<Font>;
};

} // namespace BNES::SDL

template <> struct std::hash<BNES::SDL::Font> {
  std::size_t operator()(const BNES::SDL::Font &h) const noexcept { return BNES::SDL::Font::Hash(h.name, h.variant); }
};

#endif // FONT_H
