//
// Created by nikola on 3/10/2021.
//

#include "interpreter.h"
#include "variant_match.h"

namespace freezing::interpreter {

namespace detail {

DataType operator+(const DataType& lhs, const DataType& rhs) {
  return std::visit([](auto&& lhs, auto&& rhs) -> DataType { return lhs + rhs; }, lhs, rhs);
}

DataType operator-(const DataType& lhs, const DataType& rhs) {
  return std::visit([](auto&& lhs, auto&& rhs) -> DataType { return lhs - rhs; }, lhs, rhs);
}

// Unary -
DataType operator-(const DataType& value) {
  return std::visit([](auto&& value) -> DataType { return -value; }, value);
}

DataType operator*(const DataType& lhs, const DataType& rhs) {
  return std::visit([](auto&& lhs, auto&& rhs) -> DataType { return lhs * rhs; }, lhs, rhs);
}

Result<DataType, InterpreterError> operator/(const DataType& lhs, const DataType& rhs) {
  return std::visit([](auto&& lhs, auto&& rhs) -> Result<DataType, InterpreterError> {
    if (rhs != 0) {
      return lhs / rhs;
    }
    return make_error(InterpreterError{"Invalid division by zero"});;
  }, lhs, rhs);
}

static Result<DataType, InterpreterError> Calculate(DataType left, TokenType token_type, DataType right) {
  switch (token_type) {
  case TokenType::MINUS:
    return left - right;
  case TokenType::PLUS:
    return left + right;
  case TokenType::MUL:
    return left * right;
  case TokenType::INTEGER_DIV:
  case TokenType::REAL_DIV:
    return left / right;
  case TokenType::OPEN_BRACKET:
  case TokenType::CLOSED_BRACKET:
  case TokenType::END_OF_FILE:
  case TokenType::INTEGER:
  case TokenType::DOT:
  case TokenType::BEGIN:
  case TokenType::END:
  case TokenType::ASSIGN:
  case TokenType::SEMICOLON:
  case TokenType::ID:
  case TokenType::INTEGER_CONST:
  case TokenType::REAL_CONST:
  case TokenType::COLON:
  case TokenType::PROGRAM:
  case TokenType::VAR:
  case TokenType::REAL:
  case TokenType::COMMA:
  case TokenType::PROCEDURE:
    break;
  }
  assert(false);
  return make_error(InterpreterError{"assertion"});
}

static DataType UnaryCalculate(DataType result, TokenType token_type) {
  switch (token_type) {
  case TokenType::MINUS:
    return -result;
  case TokenType::PLUS:
    return result;
  case TokenType::END_OF_FILE:
  case TokenType::INTEGER:
  case TokenType::MUL:
  case TokenType::INTEGER_DIV:
  case TokenType::OPEN_BRACKET:
  case TokenType::CLOSED_BRACKET:
  case TokenType::DOT:
  case TokenType::BEGIN:
  case TokenType::END:
  case TokenType::ASSIGN:
  case TokenType::SEMICOLON:
  case TokenType::ID:
  case TokenType::INTEGER_CONST:
  case TokenType::REAL_CONST:
  case TokenType::COLON:
  case TokenType::PROGRAM:
  case TokenType::VAR:
  case TokenType::REAL:
  case TokenType::REAL_DIV:
  case TokenType::COMMA:
  case TokenType::PROCEDURE:
    break;
  }
  assert(false);
  return -1;
}

}

InterpreterResult<ProgramState> Interpreter::run(std::string&& text) {
  auto parser = Parser::create(text);
  if (!parser) {
    return forward_error(std::move(parser));
  }
  auto program = parser->parse_program();
  if (!program) {
    return forward_error(std::move(program));
  }

  auto symbol_tables = SemanticAnalyser{}.analyse(text, *program);
  if (!symbol_tables) {
    program_state_.errors
        .insert(program_state_.errors.end(), symbol_tables.error().begin(), symbol_tables.error().end());
    return program_state_;
  }
  program_state_.symbol_tables = std::move(*symbol_tables);
  call_stack_.push(StackFrame{program->name, {}});
  auto result = process(program->block.compound_statement);
  if (!result) {
    return forward_error(std::move(result));
  }
  pop_call_stack();
  assert(call_stack_.empty());
  return program_state_;
}

InterpreterResult<Void> Interpreter::process(const CompoundStatement& compound_statement) {
  struct ProcessStatementFn {
    Interpreter& self;

    InterpreterResult<Void> operator()(const ProcedureCall& procedure_call) {
      return self.process(procedure_call);
    }

    InterpreterResult<Void> operator()(const CompoundStatement& compound_statement) {
      return self.process(compound_statement);
    }

    InterpreterResult<Void> operator()(const AssignmentStatement& assignment_statement) {
      return self.process(assignment_statement);
    }

    InterpreterResult<Void> operator()(const Empty& empty) {
      return {};
    }
  };

  for (const auto& statement : compound_statement.statements) {
    auto result = std::visit(ProcessStatementFn{*this}, statement);
    if (!result) {
      return forward_error(std::move(result));
    }
  }
  return {};
}

InterpreterResult<Void> Interpreter::process(const ProcedureCall& procedure_call) {
  StackFrame stack_frame{procedure_call.name, {}};
  // call_stack_ is guaranteed to have at least one element (main).
  assert(!call_stack_.empty() && "Call stack is guaranteed to have at least one element (main scope).");
  const auto symbol_table = program_state_.symbol_tables[call_stack_.top().scope_name];
  auto procedure_symbol = symbol_table.find_procedure_header(procedure_call.name);
  assert(procedure_symbol.has_value() && "SemanticAnalyser guarantees that the procedure exists in the current scope.");

  assert(procedure_symbol->parameters.size() == procedure_call.parameters.size()
             && "SemanticAnalyser guarantees that the number of passed arguments is equal to the number of formal parameters.");
  for (int param_idx = 0; param_idx < procedure_symbol->parameters.size(); param_idx++) {
    const auto& formal_param = procedure_symbol->parameters[param_idx];
    auto expr_result = eval(procedure_call.parameters[param_idx]);
    if (!expr_result) {
      return forward_error(std::move(expr_result));
    }
    stack_frame.variables[formal_param.identifier] = *expr_result;
  }
  call_stack_.push(std::move(stack_frame));
  auto result = process(procedure_symbol->block->compound_statement);
  pop_call_stack();
  return result;
}

InterpreterResult<Void> Interpreter::process(const AssignmentStatement& assignment_statement) {
  auto result = eval(assignment_statement.expression);
  if (!result) {
    return forward_error(std::move(result));
  }
  call_stack_.top().variables[assignment_statement.variable.name] = *result;
  return {};
  // TODO: free store memory is not supported yet.
//  std::string address = address_of(call_stack_.top().scope_name, assignment_statement.variable.name);
//  program_state_.memory.set(address, *result);
}

InterpreterResult<DataType> Interpreter::eval(const ExpressionNode& expression_node) {
  struct ExpressionNodeEvalFn {
    Interpreter& self;

    InterpreterResult<DataType> operator()(const BinOp& bin_op) {
      auto left = self.eval(*bin_op.left);
      if (!left) {
        return forward_error(std::move(left));
      }
      auto right = self.eval(*bin_op.right);
      if (!right) {
        return forward_error(std::move(right));
      }
      return detail::Calculate(*left, bin_op.op_type, *right);
    }

    InterpreterResult<DataType> operator()(const UnaryOp& unary_op) {
      auto result = self.eval(*unary_op.node);
      if (!result) {
        return forward_error(std::move(result));
      }
      return detail::UnaryCalculate(*result, unary_op.op_type);
    }

    InterpreterResult<DataType> operator()(const Variable& variable) {
      auto value = self.read_variable_value(variable.name);
      if (!value) {
        // SemanticAnalyser is responsible for ensuring that the variable is declared.
        // However, it doesn't ensure that it is initialized.
        // Therefore, at this point if the variable doesn't exist in memory, then it is uninitialized.
        return make_error(InterpreterError{
            fmt::format("Cannot read uninitialized variable '{}' in scope '{}'",
                        variable.name,
                        self.call_stack_.top().scope_name)});
      }
      return *value;
    }

    InterpreterResult<DataType> operator()(const Num& num) {
      return num.value;
    }
  };

  return std::visit(ExpressionNodeEvalFn{*this}, expression_node);
}

std::string Interpreter::address_of(const std::string& scope_name, const std::string& variable_name) {
  return fmt::format("{}_{}", scope_name, variable_name);
}

std::optional<DataType> Interpreter::read_variable_value(const std::string& variable_name) const {
  // TODO: Read recursively from all stack frames, rather than just from the current.
  const auto& stack_variables = call_stack_.top().variables;
  auto it = stack_variables.find(variable_name);
  if (it != stack_variables.end()) {
    return it->second;
  }

  // If the variable can't be found in the stack, search for it in the memory.
  // TODO: Current interpreter doesn't support free store memory as a concept.
//  const auto& scope_name = call_stack_.top().scope_name;
//  std::string address = address_of(scope_name, variable_name);
//  return program_state_.memory.try_read(address);
  return {};
}
void Interpreter::pop_call_stack() {
  // TODO: Print only if stack debugging is enabled.
  // Even better, invoke a callback that can be passed to interpreter.
  StackFrame stack_frame = call_stack_.top();
  std::cout << stack_frame << std::endl;
  call_stack_.pop();
}

}