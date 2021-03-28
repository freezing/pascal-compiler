//
// Created by nikola on 3/10/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
#define PASCAL_COMPILER_TUTORIAL__INTERPRETER_H

#include <string>
#include <map>
#include <variant>
#include "ast_visitor.h"
#include "result.h"
#include "parser.h"
#include "container_algo.h"
#include "symbol_table.h"

namespace freezing::interpreter {

struct ProgramState {
  // TODO: Should be renamed to GlobalMemory, and there should be a data-structure that can resolve
  // a symbol to a memory location.
  std::map<std::string, int> global_scope;
  std::map<NodeId, int> expression_evaluations;
  std::map<std::string, SymbolTable> symbol_tables;
  std::vector<std::string> errors;
};

class Interpreter {
public:
  Result<ProgramState> run(std::string&& text);
};

}

template<>
struct fmt::formatter<freezing::interpreter::Symbol>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::Symbol> {
};

#endif //PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
