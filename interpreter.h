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
#include "memory.h"
#include "symbol_table.h"

namespace freezing::interpreter {

struct InterpreterError {
  std::string message;

  friend std::ostream& operator<<(std::ostream& os, const InterpreterError& e) {
    return os << "InterpreterError: " << e.message;
  }
};

template<typename T>
using InterpreterResult = Result<T, std::variant<InterpreterError, ParserError, LexerError>>;

struct ProgramState {
  Memory memory;
  std::map<NodeId, int> expression_evaluations;
  std::map<std::string, SymbolTable> symbol_tables;
  std::vector<std::string> errors;
};

class Interpreter {
public:
  InterpreterResult<ProgramState> run(std::string&& text);
};

}

template<>
struct fmt::formatter<freezing::interpreter::Symbol>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::Symbol> {
};

#endif //PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
