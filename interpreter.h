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

namespace freezing::interpreter {

namespace detail {

static Result<int> Calculate(int left, TokenType token_type, int right) {
  switch (token_type) {
  case TokenType::MINUS:
    return left - right;
  case TokenType::PLUS:
    return left + right;
  case TokenType::MUL:
    return left * right;
  case TokenType::INTEGER_DIV:
  case TokenType::REAL_DIV:
    if (right == 0) {
      return make_error<std::string>("Invalid division by zero");
    }
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
    break;
  }
  assert(false);
  return make_error<std::string>("assertion");
}

static int UnaryCalculate(int result, TokenType token_type) {
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
    break;
  }
  assert(false);
  return -1;
}

struct NumTypeAsDoubleExtractorFn {
  template<typename T>
  double operator()(const T& number) const {
    return number;
  }
};

}

struct ProgramState {
  std::map<std::string, int> global_scope;
  std::map<NodeId, int> expression_evaluations;
  std::vector<std::string> errors;
};

class Interpreter {
public:
  Result<ProgramState> run(std::string&& text) {
    auto program = Parser{std::move(text)}.parse_program();
    if (!program) {
      return forward_error(std::move(program));
    }

    ProgramState program_state;

    AstVisitorCallbacks callbacks{};

    callbacks.program = [](const Program& program) {};
    callbacks.block = [](const Block& block) {};
    callbacks.var_decl = [&program_state](const VarDecl& var_decl) {
      // TODO: Implement.
    };
    callbacks.compound_statement = [](const CompoundStatement& compound_statement) {};
    callbacks.assignment_statement = [&program_state](const AssignmentStatement& assignment_statement) {
      program_state.global_scope[assignment_statement.variable.name] = program_state.expression_evaluations[node_id(assignment_statement.expression)];
    };
    callbacks.unary_op = [&program_state](const UnaryOp& unary_op) {
      int expression_value = program_state.expression_evaluations[node_id(*unary_op.node)];
      program_state.expression_evaluations[unary_op.id] = detail::UnaryCalculate(expression_value, unary_op.op_type);
    };
    callbacks.bin_op = [&program_state](const BinOp& bin_op) {
      auto left = program_state.expression_evaluations[node_id(*bin_op.left)];
      auto right = program_state.expression_evaluations[node_id(*bin_op.right)];
      auto result = detail::Calculate(left, bin_op.op_type, right);
      if (!result) {
        program_state.errors.push_back(std::move(result).error());
      } else {
        program_state.expression_evaluations[bin_op.id] = *result;
      }
    };
    callbacks.variable = [&program_state](const Variable& variable) {
      auto it = program_state.global_scope.find(variable.name);
      if (it == program_state.global_scope.end()) {
        program_state.errors.push_back(fmt::format("Unknown value for variable: '{}'", variable.name));
        return;
      }
      program_state.expression_evaluations[variable.id] = it->second;
    };
    callbacks.num = [&program_state](const Num& num) {
      // TODO: Handle float vs int.
      program_state.expression_evaluations[num.id] = std::visit(detail::NumTypeAsDoubleExtractorFn{}, num.value);
    };
    callbacks.empty = [](const Empty& empty) {};

    AstVisitorFn{callbacks}.visit(*program);
    return program_state;
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
