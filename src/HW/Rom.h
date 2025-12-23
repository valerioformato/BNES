//
// Created by vformato on 12/23/25.
//

#ifndef ROM_H
#define ROM_H

#include "common/Utils.h"

#include <cstdint>
#include <string_view>
#include <vector>

namespace BNES::HW {

class Rom {
public:
  enum class Mirroring {
    Vertical,
    Horizontal,
    FourScreen,
  };

  std::vector<uint8_t> program_rom;
  std::vector<uint8_t> character_rom;
  uint8_t mapper;
  Mirroring mirroring{Mirroring::Vertical};

  static ErrorOr<Rom> FromFile(std::string_view path);
};
} // namespace BNES::HW

#endif // ROM_H
