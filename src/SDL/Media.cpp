#include "SDL/Media.h"

namespace fs = std::filesystem;

namespace BNES::SDL::Media {
ErrorOr<SurfaceHandle> LoadBMP(fs::path path) {
  if (fs::exists(path) == false) {
    return make_error(std::make_error_code(std::errc::no_such_file_or_directory),
                      "File does not exist: " + path.string());
  }

  if (auto surface = SDL_LoadBMP(path.string().c_str()); surface != nullptr) {
    return SurfaceHandle{surface};
  } else {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }
}

} // namespace BNES::SDL::Media

BNES::ErrorOr<void> BNES::SDL::SurfaceHandle::BlitToSurface(SurfaceHandle &destination) const {
  bool result = SDL_BlitSurface(Handle(), nullptr, destination.Handle(), nullptr);

  if (result == false) {
    return make_error(std::make_error_code(std::errc::io_error), SDL_GetError());
  }

  return {};
}
