//
// Created by nikola on 3/5/2021.
//

#include "parser.h"
#include "result.h"

namespace freezing::interpreter {

namespace detail {

Result<Num> parse_integer_const(IdGenerator<NodeId>& node_id_generator, const std::string& value) {
  // TODO: Handle exceptions.
  return Num{node_id_generator.next(), std::stoi(value)};
}

Result<Num> parse_double_const(IdGenerator<NodeId>& node_id_generator, const std::string& value) {
  // TODO: Handle exceptions.
  return Num{node_id_generator.next(), std::stod(value)};
}

}

Parser::Parser(std::string&& text) : lexer_{std::move(text)}, node_id_generator{} {}

Result<Program> Parser::parse_program() {
  auto program_check = lexer_.advance(TokenType::PROGRAM);
  if (!program_check) {
    return forward_error(std::move(program_check));
  }

  auto identifier = parse_identifier();
  if (!identifier) {
    return forward_error(std::move(identifier));
  }

  auto semi_check = lexer_.advance(TokenType::SEMICOLON);
  if (!semi_check) {
    return forward_error(std::move(semi_check));
  }

  auto block = parse_block();
  if (!block) {
    return forward_error(std::move(block));
  }

  auto dot_check = lexer_.advance(TokenType::DOT);
  if (!dot_check) {
    return forward_error(std::move(dot_check));
  }
  return Program{node_id_generator.next(), std::move(*identifier), std::move(*block)};
}

Result<Block> Parser::parse_block() {
  std::vector<VarDecl> variable_declarations;
  std::vector<ProcedureDecl> procedure_declarations;

  auto current_token = lexer_.peek();
  if (!current_token) {
    return forward_error(std::move(current_token));
  }

  if (current_token->token_type == TokenType::VAR) {
    auto variable_declarations_result = parse_variable_declarations();
    if (!variable_declarations_result) {
      return forward_error(std::move(variable_declarations_result));
    }
    variable_declarations = std::move(*variable_declarations_result);
  }

  current_token = lexer_.peek();
  if (!current_token) {
    return forward_error(std::move(current_token));
  }

  if (current_token->token_type == TokenType::PROCEDURE) {
    auto procedure_declarations_result = parse_procedure_declarations();
    if (!procedure_declarations_result) {
      return forward_error(std::move(procedure_declarations_result));
    }
    procedure_declarations = std::move(*procedure_declarations_result);
  }

  auto compound_statement = parse_compound_statement();
  if (!compound_statement) {
    return forward_error(std::move(compound_statement));
  }

  return Block{node_id_generator.next(), std::move(variable_declarations), std::move(procedure_declarations),
               std::move(*compound_statement)};
}

Result<std::vector<VarDecl>> Parser::parse_variable_declarations() {
  auto var_check = lexer_.advance(TokenType::VAR);
  if (!var_check) {
    return forward_error(std::move(var_check));
  }
  std::vector<VarDecl> declarations;
  while (true) {
    auto variable_declaration = parse_variable_declaration();
    if (!variable_declaration) {
      return forward_error(std::move(variable_declaration));
    }
    auto semi_check = lexer_.advance(TokenType::SEMICOLON);
    if (!semi_check) {
      return forward_error(std::move(semi_check));
    }

    declarations.push_back(std::move(*variable_declaration));

    const auto& next_token = lexer_.peek();
    if (!next_token) {
      return forward_error(lexer_.pop());
    }
    if (next_token->token_type != TokenType::ID) {
      break;
    }
  }
  return declarations;
}

Result<VarDecl> Parser::parse_variable_declaration() {
  auto variable = parse_variable();
  if (!variable) {
    return forward_error(std::move(variable));
  }

  std::vector<Variable> variables;
  variables.push_back(std::move(*variable));

  while (true) {
    auto next_token = lexer_.peek();
    if (!next_token) {
      return forward_error(std::move(next_token));
    }
    if (next_token->token_type != TokenType::COMMA) {
      break;
    }

    lexer_.advance();
    auto new_variable = parse_variable();
    if (!new_variable) {
      return forward_error(std::move(new_variable));
    }
    variables.push_back(std::move(*new_variable));
  }

  auto colon_check = lexer_.advance(TokenType::COLON);
  if (!colon_check) {
    return forward_error(std::move(colon_check));
  }

  auto type_spec = parse_type();
  if (!type_spec) {
    return forward_error(std::move(type_spec));
  }
  return VarDecl{node_id_generator.next(), std::move(variables), *type_spec};
}

Result<std::vector<ProcedureDecl>> Parser::parse_procedure_declarations() {
  auto current_token = lexer_.peek();
  if (!current_token) {
    return forward_error(std::move(current_token));
  }

  std::vector<ProcedureDecl> procedure_declarations;
  while (current_token->token_type == TokenType::PROCEDURE) {
    lexer_.advance();

    auto name = parse_identifier();
    if (!name) {
      return forward_error(std::move(name));
    }

    auto semi_check = lexer_.advance(TokenType::SEMICOLON);
    if (!semi_check) {
      return forward_error(std::move(semi_check));
    }

    auto block = parse_block();
    if (!block) {
      return forward_error(std::move(block));
    }

    auto final_semi_check = lexer_.advance(TokenType::SEMICOLON);
    if (!final_semi_check) {
      return forward_error(std::move(final_semi_check));
    }

    procedure_declarations.push_back(ProcedureDecl{node_id_generator.next(), std::move(*name), std::move(*block)});

    current_token = lexer_.peek();
    if (!current_token) {
      return forward_error(std::move(current_token));
    }
  }
  return procedure_declarations;
}

Result<TokenType> Parser::parse_type() {
  auto token = lexer_.pop();
  if (!token) {
    return forward_error(std::move(token));
  }
  if (token->token_type != TokenType::INTEGER && token->token_type != TokenType::REAL) {
    return make_error(fmt::format("Expected type 'INTEGER' or 'REAL', but got token: '{}'", *token));
  }
  return token->token_type;
}

Result<CompoundStatement> Parser::parse_compound_statement() {
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
  return CompoundStatement{node_id_generator.next(), std::move(*statements)};
}

Result<std::vector<Statement>> Parser::parse_statement_list() {
  std::vector<Statement> statements{};

  {
    auto statement = parse_statement();
    if (!statement) {
      return forward_error(std::move(statement));
    }
    statements.push_back((std::move(*statement)));
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
    statements.push_back(std::move(*statement));
  }
  return statements;
}

Result<Statement> Parser::parse_statement() {
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

Result<AssignmentStatement> Parser::parse_assignment_statement() {
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

  return AssignmentStatement{node_id_generator.next(), std::move(*variable), std::move(*expr)};
}

Result<Identifier> Parser::parse_identifier() {
  auto token = lexer_.pop(TokenType::ID);
  if (!token) {
    return forward_error(std::move(token));
  }

  return std::move(token).map([](Token&& token) {
    return std::move(*token.value);
  });
}

Result<Variable> Parser::parse_variable() {
  auto identifier = parse_identifier();
  if (!identifier) {
    return forward_error(std::move(identifier));
  }

  return Variable{node_id_generator.next(), std::move(*identifier)};
}

Empty Parser::parse_empty() {
  return Empty{node_id_generator.next()};
}

Result<ExpressionNode> Parser::parse_expr() {
  auto first_term = parse_term();
  if (!first_term) {
    return forward_error(std::move(first_term));
  }
  ExpressionNode result = std::move(*first_term);

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

    result = BinOp{node_id_generator.next(),
                   op_token->token_type,
                   std::make_unique<ExpressionNode>(std::move(result)),
                   std::make_unique<ExpressionNode>(std::move(*right))};
  }
  return result;
}

Result<ExpressionNode> Parser::parse_term() {
  auto first_factor = parse_factor();
  if (!first_factor) {
    return forward_error(std::move(first_factor));
  }

  ExpressionNode result = std::move(*first_factor);

  while (true) {
    auto op_token = lexer_.peek();
    if (!op_token) {
      return forward_error(std::move(op_token));
    }

    if (op_token->token_type != TokenType::MUL && op_token->token_type != TokenType::INTEGER_DIV
        && op_token->token_type != TokenType::REAL_DIV) {
      break;
    }
    lexer_.advance();

    auto right = parse_factor();
    if (!right) {
      return forward_error(std::move(right));
    }

    result = BinOp{node_id_generator.next(),
                   op_token->token_type,
                   std::make_unique<ExpressionNode>(std::move(result)),
                   std::make_unique<ExpressionNode>(std::move(*right))};
  }
  return result;
}

Result<ExpressionNode> Parser::parse_factor() {
  auto first_token = lexer_.peek();
  if (!first_token) {
    return forward_error(std::move(first_token));
  }

  auto unary_op_handler = [this, &first_token](Token&& token) -> Result<ExpressionNode> {
    auto make_ast = [this, &first_token](ExpressionNode&& factor) -> ExpressionNode {
      return UnaryOp{node_id_generator.next(), first_token->token_type,
                     std::make_unique<ExpressionNode>(std::move(factor))};
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

  // INTEGER_CONST
  // REAL_CONST
  // ID
  if (token->token_type == TokenType::INTEGER_CONST) {
    auto integer_check = lexer_.advance(TokenType::INTEGER_CONST);
    if (!integer_check) {
      return forward_error(std::move(integer_check));
    }
    return detail::parse_integer_const(node_id_generator, *token->value);
  } else if (token->token_type == TokenType::REAL_CONST) {
    auto real_check = lexer_.advance(TokenType::REAL_CONST);
    if (!real_check) {
      return forward_error(std::move(real_check));
    }
    return detail::parse_double_const(node_id_generator, *token->value);
  } else if (token->token_type == TokenType::ID) {
    return parse_variable();
  } else {
    return make_error(fmt::format("Unexpected token {} found while trying to parse factor grammar.",
                                  token->token_type));
  }
}

}