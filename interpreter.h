//
// Created by nikola on 3/10/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
#define PASCAL_COMPILER_TUTORIAL__INTERPRETER_H

#include <stack>
#include <string>
#include <map>
#include <variant>
#include "stack_frame.h"
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
  // TODO: Rename to scopes.
  std::map<std::string, SymbolTable> symbol_tables;
  std::vector<InterpreterErrorsT> errors;
};

class Interpreter {
public:
  InterpreterResult<ProgramState> run(std::string&& text);

private:
  ProgramState program_state_;
  std::stack<StackFrame> call_stack_;

  InterpreterResult<Void> process(const ProcedureCall& procedure_call);
  InterpreterResult<Void> process(const CompoundStatement& compound_statement);
  InterpreterResult<Void> process(const AssignmentStatement& assignment_statement);
  InterpreterResult<DataType> eval(const ExpressionNode& expression_node);

  std::optional<DataType> read_variable_value(const std::string& value) const;
  void pop_call_stack();

  static std::string address_of(const std::string& scope_name, const std::string& variable_name);
};

}

template<>
struct fmt::formatter<freezing::interpreter::Symbol>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::Symbol> {
};

#endif //PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
