//
// Created by vformato on 7/21/25.
//

#ifndef ENUMARRAY_H
#define ENUMARRAY_H

#include <magic_enum.hpp>

#include <array>

namespace BNES {

template <typename ContainedClass, typename Enum>
class EnumArray : public std::array<ContainedClass, magic_enum::enum_count<Enum>()> {
public:
  static constexpr auto EnumSize = magic_enum::enum_count<Enum>();
  using BaseType = std::array<ContainedClass, EnumSize>;

  // constructors
  EnumArray() = default;
  EnumArray(const EnumArray<ContainedClass, Enum> &c) : BaseType(c) {}
  EnumArray(const std::array<ContainedClass, EnumSize> &values) : BaseType(values) {}
  EnumArray(std::array<ContainedClass, EnumSize> &&values) : BaseType(std::move(values)) {}

  // assignment
  EnumArray<ContainedClass, Enum> &operator=(const EnumArray<ContainedClass, Enum> &lhs) {
    BaseType::operator=(lhs);
    return *this;
  }

  // access
  ContainedClass &operator[](const Enum &index) {
    return BaseType::operator[](static_cast<std::underlying_type_t<Enum>>(index));
  }
  const ContainedClass &operator[](const Enum &index) const {
    return BaseType::operator[](static_cast<std::underlying_type_t<Enum>>(index));
  }
  ContainedClass &at(const Enum &index) { return BaseType::at(static_cast<std::underlying_type_t<Enum>>(index)); }
  const ContainedClass &at(const Enum &index) const {
    return BaseType::at(static_cast<std::underlying_type_t<Enum>>(index));
  }
};

} // namespace BNES

#endif // ENUMARRAY_H
