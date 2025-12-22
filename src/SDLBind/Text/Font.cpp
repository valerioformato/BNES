//
// Created by vformato on 12/22/25.
//

#include "SDLBind/Text/Font.h"

#include <spdlog/spdlog.h>

namespace BNES::SDL {
namespace {
std::unordered_set<Font> Fonts;
}

ErrorOr<Font> Font::FromFile(std::string name, FontVariant variant) {
  static std::filesystem::path path{"assets/fonts/"};

  auto file_name = fmt::format("{}-{}.ttf", name, magic_enum::enum_name(variant));
  auto font_path = path / file_name;

  if (!std::filesystem::exists(font_path)) {
    return make_error(std::errc::no_such_file_or_directory, fmt::format("Font does not exist: {}", font_path.string()));
  }

  if (auto font = TTF_OpenFont(font_path.string().c_str(), 14); font == nullptr) {
    return make_error(std::errc::no_such_file_or_directory, fmt::format("Cannot load font: {}", SDL_GetError()));
  } else {
    auto [handle, result] = Fonts.insert(Font{
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
