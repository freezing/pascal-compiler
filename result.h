//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__RESULT_H
#define PASCAL_COMPILER_TUTORIAL__RESULT_H

#include <tl/expected.hpp>
#include <string>

struct Void {};

template<typename T, typename E = std::string>
using Result = tl::expected<T, E>;

template<typename E>
using Error = tl::unexpected<E>;

template<typename E>
Error<E> make_error(E&& e) {
  return Error<typename std::decay<E>::type>(std::forward<E>(e));
}

template<typename T1, typename E>
Error<E> forward_error(const Result<T1, E>& result) {
  return make_error(result.error());
}

template<typename T1, typename E>
Error<E> forward_error(Result<T1, E>&& result) {
  return make_error(std::move(result).error());
}

#endif //PASCAL_COMPILER_TUTORIAL__RESULT_H
