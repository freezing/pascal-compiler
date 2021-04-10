//
// Created by nikola on 4/10/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__STACK_FRAME_H
#define PASCAL_COMPILER_TUTORIAL__STACK_FRAME_H

#include <iostream>
#include <fmt/format.h>
#include "memory.h"

namespace freezing::interpreter {

struct StackFrame {
  std::string scope_name;
  std::map<std::string, DataType> variables;

  friend std::ostream& operator<<(std::ostream& os, const StackFrame& frame) {
    os << fmt::format("StackFrame({})", frame.scope_name) << std::endl;
    for (const auto& variable : frame.variables) {
      os << "  " << variable.first << " = " << variable.second << std::endl;
    }
    return os;
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__STACK_FRAME_H
