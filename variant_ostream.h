//
// Created by nikola on 4/3/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__VARIANT_OSTREAM_H
#define PASCAL_COMPILER_TUTORIAL__VARIANT_OSTREAM_H

#include <iostream>
#include "stringstream_formatter.h"

namespace freezing::interpreter {

//template<typename...Ts>
//inline std::ostream& operator<<(std::ostream& os, const std::variant<Ts...>& v) {
//  return std::visit([&os](auto&& arg) { return os << arg; }, v);
//}

namespace detail {

struct VariantOstreamVisitFn {
  std::ostream& os;

  template<typename T>
  std::ostream& operator()(const T& value) {
    return os << value;
  }
};

}

template<typename H, typename...Ts>
inline std::ostream& operator<<(std::ostream& os, const std::variant<H, Ts...>& value) {
  return std::visit(detail::VariantOstreamVisitFn{os}, value);
}

}

template<typename H, typename...Ts>
struct fmt::formatter<std::variant<H, Ts...>> : freezing::interpreter::StringStreamFormatter<std::variant<H, Ts...>> {};

#endif //PASCAL_COMPILER_TUTORIAL__VARIANT_OSTREAM_H
