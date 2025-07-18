#include "SDL/Media.h"

namespace fs = std::filesystem;

namespace BNES::SDL::Media {
ErrorOr<SurfaceHandle> LoadBMP(fs::path path) {
  if (fs::exists(path) == false) {
    return make_error(std::make_error_code(std::errc::no_such_file_or_directory),
                      "File does not exist: " + path.string());
  }

  if (auto surface = SDL_LoadBMP(path.c_str()); surface != nullptr) {
    return SurfaceHandle{surface};
  } else {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }
}

} // namespace BNES::SDL::Media
