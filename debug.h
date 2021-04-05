//
// Created by nikola on 4/5/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__DEBUG_H
#define PASCAL_COMPILER_TUTORIAL__DEBUG_H

#include <string>
#include <iomanip>
#include "token.h"

namespace freezing::interpreter {

// Returns a debug string to print around the given location.
std::string debug_output(const std::string& text, CharLocation location);

}

#endif //PASCAL_COMPILER_TUTORIAL__DEBUG_H
