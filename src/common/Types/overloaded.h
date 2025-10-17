//
// Created by vformato on 10/17/25.
//

#ifndef OVERLOADED_H
#define OVERLOADED_H

namespace BNES::Utils {
template <class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;
} // namespace BNES::Utils

#endif // OVERLOADED_H
