//
// Created by nikola on 3/5/2021.
//

#include "parser.h"

namespace freezing::interpreter {

Parser::Parser(std::string&& text) : lexer_{std::move(text)} {}

Result<AstNode> Parser::parse_program() {
  auto compound_statement = parse_compound_statement();
  if (!compound_statement) {
    return forward_error(std::move(compound_statement));
  }
  auto dot_check = lexer_.advance(TokenType::DOT);
  if (!dot_check) {
    return forward_error(std::move(dot_check));
  }
  return compound_statement;
}

Result<AstNode> Parser::parse_compound_statement() {
  auto begin_check = lexer_.advance(TokenType::BEGIN);
  if (!begin_check) {
    return forward_error(std::move(begin_check));
  }

  auto statements = parse_statement_list();
  if (!statements) {
    return forward_error(std::move(statements));
  }

  auto end_check = lexer_.advance(TokenType::END);
  if (!end_check) {
    return forward_error(std::move(end_check));
  }
  return AstNode{CompoundStatement{std::move(std::get<StatementList>(statements->value))}};
}

Result<AstNode> Parser::parse_statement_list() {
  StatementList list{};

  {
    auto statement = parse_statement();
    if (!statement) {
      return forward_error(std::move(statement));
    }
    list.statements.push_back(std::make_unique<AstNode>(std::move(*statement)));
  }

  while (true) {
    auto next_token = lexer_.peek();
    if (!next_token) {
      return forward_error(std::move(next_token));
    }

    if (next_token->token_type != TokenType::SEMICOLON) {
      break;
    }
    auto semicolon_check = lexer_.advance(TokenType::SEMICOLON);
    assert(semicolon_check.has_value());

    auto statement = parse_statement();
    if (!statement) {
      return forward_error(std::move(statement));
    }
    list.statements.push_back(std::make_unique<AstNode>(std::move(*statement)));
  }
  return AstNode{std::move(list)};
}

Result<AstNode> Parser::parse_statement() {
  auto next_token = lexer_.peek();
  if (!next_token) {
    return forward_error(std::move(next_token));
  }

  if (next_token->token_type == TokenType::BEGIN) {
    return parse_compound_statement();
  } else if (next_token->token_type == TokenType::ID) {
    return parse_assignment_statement();
  } else {
    return parse_empty();
  }
}

Result<AstNode> Parser::parse_assignment_statement() {
  auto variable = parse_variable();
  if (!variable) {
    return forward_error(std::move(variable));
  }

  auto check_assign = lexer_.advance(TokenType::ASSIGN);
  if (!check_assign) {
    return forward_error(std::move(check_assign));
  }

  auto expr = parse_expr();
  if (!expr) {
    return forward_error(std::move(expr));
  }

  return AstNode{AssignmentStatement{std::make_unique<AstNode>(AstNode{std::move(*variable)}),
                                     std::make_unique<AstNode>(std::move(*expr))}};
}

Result<Variable> Parser::parse_variable() {
  auto token = lexer_.peek();
  if (!token) {
    return forward_error(std::move(token));
  }

  auto check_id = lexer_.advance(TokenType::ID);
  if (!check_id) {
    return forward_error(std::move(check_id));
  }

  return std::move(token).map([](Token&& token) {
    return Variable{std::move(token)};
  });
}

AstNode Parser::parse_empty() {
  return AstNode{Empty{}};
}

Result<AstNode> Parser::parse_expr() {
  auto first_term = parse_term();
  if (!first_term) {
    return forward_error(std::move(first_term));
  }
  AstNode result = std::move(*first_term);

  while (true) {
    auto op_token = lexer_.peek();
    if (!op_token) {
      return forward_error(std::move(op_token));
    }

    if (op_token->token_type != TokenType::PLUS && op_token->token_type != TokenType::MINUS) {
      break;
    }
    lexer_.advance();

    auto right = parse_term();
    if (!right) {
      return forward_error(std::move(right));
    }

    result = AstNode{BinOp{*op_token,
                           std::make_unique<AstNode>(std::move(result)),
                           std::make_unique<AstNode>(std::move(*right))}};
  }
  return result;
}

Result<AstNode> Parser::parse_term() {
  auto first_factor = parse_factor();
  if (!first_factor) {
    return forward_error(std::move(first_factor));
  }

  AstNode result = std::move(*first_factor);

  while (true) {
    auto op_token = lexer_.peek();
    if (!op_token) {
      return forward_error(std::move(op_token));
    }

    if (op_token->token_type != TokenType::MUL && op_token->token_type != TokenType::DIV) {
      break;
    }
    lexer_.advance();

    auto right = parse_factor();
    if (!right) {
      return forward_error(std::move(right));
    }

    result = AstNode{BinOp{*op_token,
                           std::make_unique<AstNode>(std::move(result)),
                           std::make_unique<AstNode>(std::move(*right))}};
  }
  return result;
}

Result<AstNode> Parser::parse_factor() {
  auto first_token = lexer_.peek();
  if (!first_token) {
    return forward_error(std::move(first_token));
  }

  auto unary_op_handler = [this, &first_token](Token&& token) -> Result<AstNode> {
    auto make_ast = [&first_token](AstNode&& factor) -> AstNode {
      return AstNode{UnaryOp{std::move(*first_token), std::make_unique<AstNode>(std::move(factor))}};
    };
    return lexer_.advance(token.token_type)
        .and_then([this](auto&&) { return parse_factor(); })
        .map(make_ast);
  };

  // PLUS factor
  // MINUS factor
  if (first_token->token_type == TokenType::PLUS || first_token->token_type == TokenType::MINUS) {
    return std::invoke(unary_op_handler, std::move(*first_token));
  }

  // LPAREN expr RPAREN
  if (first_token->token_type == TokenType::OPEN_BRACKET) {
    auto advance_check = lexer_.advance(TokenType::OPEN_BRACKET);
    if (!advance_check) {
      return forward_error(std::move(advance_check));
    }

    auto expression = parse_expr();
    if (!expression) {
      return forward_error(std::move(expression));
    }

    // Check that the next token is CLOSED_BRACKET
    auto closed_bracket = lexer_.pop(TokenType::CLOSED_BRACKET);
    if (!closed_bracket) {
      return forward_error(std::move(closed_bracket));
    }
    return expression;
  }

  // TODO: Lexer peek and advance should be cleaned up.
  auto token = lexer_.peek();
  if (!token) {
    return forward_error(std::move(token));
  }

  // INTEGER
  // ID
  if (token->token_type == TokenType::INTEGER) {
    auto integer_check = lexer_.advance(TokenType::INTEGER);
    if (!integer_check) {
      return forward_error(std::move(integer_check));
    }
    return AstNode{Num{*token}};
  } else if (token->token_type == TokenType::ID) {
    auto num_check = lexer_.advance(TokenType::ID);
    if (!num_check) {
      return forward_error(std::move(num_check));
    }
    return AstNode{Variable{*token}};
  } else {
    return make_error(fmt::format("Unexpected token {} found while trying to parse factor grammar.",
                                  token->token_type));
  }
}

}