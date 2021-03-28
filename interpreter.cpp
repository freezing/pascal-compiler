//
// Created by nikola on 3/10/2021.
//

#include "interpreter.h"
#include "semantic_analyser.h"

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
  case TokenType::PROCEDURE:
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
  case TokenType::PROCEDURE:
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

static void add_error_if_variable(std::vector<std::string>& errors, const ExpressionNode& expression_node) {
  struct Fn {
    std::vector<std::string>& errors;

    void operator()(const BinOp& bin_op) const {}

    void operator()(const UnaryOp& unary_op) const {}

    void operator()(const Variable& variable) const {
      errors.push_back(fmt::format("Unknown value for variable: '{}'", variable.name));
    }

    void operator()(const Num& num) const {}
  };
  std::visit(Fn{errors}, expression_node);
}

}

Result<ProgramState> Interpreter::run(std::string&& text) {
  auto program = Parser{std::move(text)}.parse_program();
  if (!program) {
    return forward_error(std::move(program));
  }

  ProgramState program_state;

  auto symbol_tables = SemanticAnalyser{}.analyse(*program);
  if (!symbol_tables) {
    program_state.errors.insert(program_state.errors.end(), symbol_tables.error().begin(), symbol_tables.error().end());
    return program_state;
  }
  program_state.symbol_tables = std::move(*symbol_tables);

  AstVisitorCallbacks callbacks{};

  callbacks.program_post = [](const Program& program) {};
  callbacks.block = [](const Block& block) {};
  callbacks.compound_statement = [](const CompoundStatement& compound_statement) {};
  callbacks.assignment_statement = [&program_state](const AssignmentStatement& assignment_statement) {
    auto expr_eval_it = program_state.expression_evaluations.find(node_id(assignment_statement.expression));
    if (expr_eval_it == program_state.expression_evaluations.end()) {
      return;
    }
    program_state.global_scope[assignment_statement.variable.name] = expr_eval_it->second;
  };
  callbacks.unary_op = [&program_state](const UnaryOp& unary_op) {
    int expression_value = program_state.expression_evaluations[node_id(*unary_op.node)];
    program_state.expression_evaluations[unary_op.id] = detail::UnaryCalculate(expression_value, unary_op.op_type);
  };
  callbacks.bin_op = [&program_state](const BinOp& bin_op) {
    auto left_it = program_state.expression_evaluations.find(node_id(*bin_op.left));
    auto right_it = program_state.expression_evaluations.find(node_id(*bin_op.right));

    if (left_it == program_state.expression_evaluations.end()) {
      detail::add_error_if_variable(program_state.errors, *bin_op.left);
      return;
    }
    if (right_it == program_state.expression_evaluations.end()) {
      detail::add_error_if_variable(program_state.errors, *bin_op.right);
      return;
    }

    auto result = detail::Calculate(left_it->second, bin_op.op_type, right_it->second);
    if (!result) {
      program_state.errors.push_back(std::move(result).error());
    } else {
      program_state.expression_evaluations[bin_op.id] = *result;
    }
  };
  callbacks.variable = [&program_state](const Variable& variable) {
    auto it = program_state.global_scope.find(variable.name);
    if (it == program_state.global_scope.end()) {
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
  // Remove duplicate errors.
  std::sort(program_state.errors.begin(), program_state.errors.end());
  program_state.errors
      .erase(std::unique(program_state.errors.begin(), program_state.errors.end()), program_state.errors.end());
  return program_state;
}

}