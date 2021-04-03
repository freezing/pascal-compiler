//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__RESULT_H
#define PASCAL_COMPILER_TUTORIAL__RESULT_H

#include <tl/expected.hpp>
#include <string>
#include <variant>
#include <type_traits>
#include "variant_cast.h"

namespace detail {

template<typename T> struct is_variant : std::false_type {};

template<typename ...Args>
struct is_variant<std::variant<Args...>> : std::true_type {};

}

struct Void {};

template<typename T, typename E = std::string>
using Result = tl::expected<T, E>;

template<typename E>
using Error = tl::unexpected<E>;

template<typename E>
Error<E> make_error(E&& e) {
  return Error<typename std::decay<E>::type>(std::forward<E>(e));
}

template<typename T, typename E>
Error<E> forward_error(const Result<T, E>& result) {
  return make_error(result.error());
}

template<typename T, typename E>
auto forward_error(Result<T, E>&& result) {
  if constexpr (detail::is_variant<E>::value) {
    return make_error(variant_cast(std::move(result).error()));
  } else {
    return make_error(std::move(result).error());
  }
}

#endif //PASCAL_COMPILER_TUTORIAL__RESULT_H
