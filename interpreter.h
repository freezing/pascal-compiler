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
#include "semantic_analyser.h"

namespace freezing::interpreter {

struct InterpreterError {
  std::string message;

  friend std::ostream& operator<<(std::ostream& os, const InterpreterError& e) {
    return os << "InterpreterError: " << e.message;
  }
};

using InterpreterErrorsT = std::variant<InterpreterError, SemanticAnalysisError, ParserError, LexerError>;

template<typename T>
using InterpreterResult = Result<T, InterpreterErrorsT>;

struct ProgramState {
  Memory memory;
  // TODO: This is a temporary hack, until I implement proper source-to-source compiler which uses memory to store
  // temporary values.
  // This is currently acting as a bypass for optimization stage.
  std::map<NodeId, DataType> expression_evaluations;
  std::map<std::string, SymbolTable> symbol_tables;
  std::vector<InterpreterErrorsT> errors;
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
