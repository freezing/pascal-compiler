//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__STRINGSTREAM_FORMATTER_H
#define PASCAL_COMPILER_TUTORIAL__STRINGSTREAM_FORMATTER_H

#include <sstream>
#include <fmt/format.h>

namespace freezing::interpreter {

template <typename T>
struct StringStreamFormatter : fmt::formatter<std::string> {
  template <typename FormatContext>
  auto format(const T& value, FormatContext& ctx) {
    std::stringstream ss{};
    ss << value;
    return fmt::format_to(ctx.out(), "{}", ss.str());
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__STRINGSTREAM_FORMATTER_H
