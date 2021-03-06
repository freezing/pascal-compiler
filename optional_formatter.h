//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__OPTIONAL_FORMATTER_H
#define PASCAL_COMPILER_TUTORIAL__OPTIONAL_FORMATTER_H

#include <iostream>
#include <optional>

namespace freezing::interpreter {

template<typename T>
inline std::ostream& operator<<(std::ostream& os, const std::optional<T>& an_optional) {
  if (an_optional) {
    return os << an_optional.value();
  }
  return os << "[None]";
}

}

#endif //PASCAL_COMPILER_TUTORIAL__OPTIONAL_FORMATTER_H
