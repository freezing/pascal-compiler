//
// Created by nikola on 4/3/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__VARIANT_MATCH_H
#define PASCAL_COMPILER_TUTORIAL__VARIANT_MATCH_H

#include <optional>
#include <variant>

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

template<typename T, typename...Args>
std::optional<T> variant_try_get(const std::variant<Args...>& v) noexcept {
  try {
    return std::get<T>(v);
  } catch (const std::bad_variant_access& e) {
    return {};
  }
}

#endif //PASCAL_COMPILER_TUTORIAL__VARIANT_MATCH_H
