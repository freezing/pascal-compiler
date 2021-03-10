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
  case TokenType::DIV:
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
  case TokenType::DIV:
  case TokenType::OPEN_BRACKET:
  case TokenType::CLOSED_BRACKET:
  case TokenType::DOT:
  case TokenType::BEGIN:
  case TokenType::END:
  case TokenType::ASSIGN:
  case TokenType::SEMICOLON:
  case TokenType::ID:
    break;
  }
  assert(false);
  return -1;
}
}

struct ProgramState {
  std::map<std::string, int> global_scope;
};

class Interpreter {
public:
  Result<ProgramState> run(std::string&& text) {
    auto tree = Parser{std::move(text)}.parse_program();
    if (!tree) {
      return forward_error(std::move(tree));
    }

    ProgramState program_state;

    using InterpreterState = std::variant<std::monostate, int>;

    auto compound_statement_callback = [&program_state](const AstNode* node,
                                                        const CompoundStatement& compound_statement,
                                                        std::vector<Result<InterpreterState>>&& states) -> Result<
        InterpreterState> {
      for (auto& result : states) {
        if (!result) {
          return forward_error(std::move(result));
        }
      }
      return {};
    };

    auto statement_list_callback = [&program_state](const AstNode* node,
                                                    const StatementList& statement_list,
                                                    std::vector<Result<InterpreterState>>&& states) -> Result<
        InterpreterState> {
      for (auto& result : states) {
        if (!result) {
          return forward_error(std::move(result));
        }
      }
      return {};
    };

    auto assignment_statement_callback = [&program_state](const AstNode* node,
                                                          const AssignmentStatement& assignment_statement,
                                                          Result<InterpreterState>&& eval_var,
                                                          Result<InterpreterState>&& expr_state) -> Result<
        InterpreterState> {
      if (!expr_state) {
        return forward_error(std::move(expr_state));
      }
      const auto& variable = std::get<Variable>(assignment_statement.variable->value);
      return program_state.global_scope[*variable.token.value] = std::get<int>(*expr_state);
    };

    auto empty_callback = [](const AstNode*) -> Result<InterpreterState> {
      return {};
    };

    auto num_callback = [](const AstNode* node, const Num& num) -> Result<InterpreterState> {
      return std::stoi(*num.token.value);
    };

    auto unary_op_callback =
        [](const AstNode* node, const UnaryOp& unary_op, Result<InterpreterState>&& state) -> Result<InterpreterState> {
          if (!state) {
            return forward_error(std::move(state));
          }
          int value = std::get<int>(*state);
          return detail::UnaryCalculate(value, unary_op.op.token_type);
        };

    auto bin_op_callback = [](const AstNode* node,
                              const BinOp& bin_op,
                              Result<InterpreterState>&& left,
                              Result<InterpreterState>&& right) -> Result<InterpreterState> {
      if (!left) {
        return forward_error(std::move(left));
      }
      if (!right) {
        return forward_error(std::move(right));
      }
      int left_value = std::get<int>(*left);
      int right_value = std::get<int>(*right);
      return detail::Calculate(left_value, bin_op.op.token_type, right_value);
    };

    auto variable_callback =
        [&program_state](const AstNode* node, const Variable& variable) -> Result<InterpreterState> {
          const auto& var = *variable.token.value;
          auto it = program_state.global_scope.find(var);
          if (it == program_state.global_scope.end()) {
            return make_error(fmt::format("Unknown variable: '{}'", *variable.token.value));
          }
          return it->second;
        };

    auto node_result = AstVisitorFn<Result<InterpreterState>>{
        compound_statement_callback,
        statement_list_callback,
        assignment_statement_callback,
        empty_callback,
        unary_op_callback,
        bin_op_callback,
        num_callback,
        variable_callback
    }.Visit(&tree.value());

    if (!node_result) {
      return forward_error(std::move(node_result));
    }
    return program_state;
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__INTERPRETER_H
