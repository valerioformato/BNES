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

class FontHandle {
public:
  // FontHandle() = default;
  // ~FontHandle() { TTF_CloseFont(font); }
  // FontHandle(const FontHandle &) = default;
  // FontHandle(FontHandle &&) = default;
  // FontHandle &operator=(const FontHandle &) = default;
  // FontHandle &operator=(FontHandle &&) = default;

  bool operator==(const FontHandle &other) const { return name == other.name || font == other.font; }

  std::string name;
  TTF_Font *font{nullptr};
};

inline ErrorOr<FontHandle> LoadFont(std::string name, FontVariant variant);
} // namespace BNES::SDL

template <> struct std::hash<BNES::SDL::FontHandle> {
  std::size_t operator()(const BNES::SDL::FontHandle &h) const noexcept { return std::hash<std::string>{}(h.name); }
};

namespace BNES::SDL {
static std::unordered_set<FontHandle> Fonts;

ErrorOr<FontHandle> LoadFont(std::string name, FontVariant variant) {
  static std::filesystem::path path{"assets/fonts/"};

  auto file_name = fmt::format("{}-{}.ttf", name, magic_enum::enum_name(variant));
  auto font_path = path / file_name;

  if (!std::filesystem::exists(font_path)) {
    return make_error(std::errc::no_such_file_or_directory, fmt::format("Font does not exist: {}", font_path.string()));
  }

  if (auto font = TTF_OpenFont(font_path.c_str(), 14); font == nullptr) {
    return make_error(std::errc::no_such_file_or_directory, fmt::format("Cannot load font: {}", SDL_GetError()));
  } else {
    auto [handle, result] = Fonts.insert(FontHandle{
        .name = font_path.string(),
        .font = font,
    });
    if (!result) {
      spdlog::error("Could not insert font into font {} table", file_name);
      return make_error(std::errc::not_enough_memory, fmt::format("Could not insert font {}", file_name));
    }
    return *handle;
  };
}
} // namespace BNES::SDL

#endif // FONT_H
