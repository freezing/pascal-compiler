//
// Created by nikola on 3/5/2021.
//

#include "parser.h"

#include <utility>
#include "result.h"
#include "debug.h"

namespace freezing::interpreter {

namespace detail {

ParserResult<Num> parse_integer_const(IdGenerator<NodeId>& node_id_generator, const std::string& value) {
  // TODO: Handle exceptions.
  return Num{node_id_generator.next(), std::stoi(value)};
}

ParserResult<Num> parse_double_const(IdGenerator<NodeId>& node_id_generator, const std::string& value) {
  // TODO: Handle exceptions.
  return Num{node_id_generator.next(), std::stod(value)};
}

}

Parser::Parser(std::string text, std::vector<Token>&& tokens) : text_{std::move(text)}, tokens_{std::move(tokens)},
                                                                current_token_{tokens_.begin()},
                                                                node_id_generator{} {
}

Result<Parser, LexerError> Parser::create(std::string text) {
  Lexer lexer{text};

  std::vector<Token> tokens;

  while (true) {
    auto advance_check = lexer.advance();
    if (!advance_check) {
      return forward_error(std::move(advance_check));
    }
    tokens.push_back(std::move(lexer.peek()));
    if (tokens.back().token_type == TokenType::END_OF_FILE) {
      break;
    }
  }
  assert(tokens.back().token_type == TokenType::END_OF_FILE);
  return Parser{std::move(text), std::move(tokens)};
}

ParserResult<Program> Parser::parse_program() {
  if (!is_current_token(TokenType::PROGRAM)) {
    return unexpected_token_error(TokenType::PROGRAM, *current_token_);
  }
  current_token_++;

  auto identifier = parse_identifier();
  if (!identifier) {
    return forward_error(std::move(identifier));
  }

  if (!is_current_token(TokenType::SEMICOLON)) {
    return unexpected_token_error(TokenType::SEMICOLON, *current_token_);
  }
  current_token_++;

  auto block = parse_block();
  if (!block) {
    return forward_error(std::move(block));
  }

  if (!is_current_token(TokenType::DOT)) {
    return unexpected_token_error(TokenType::DOT, *current_token_);
  }
  current_token_++;
  return Program{node_id_generator.next(), std::move(*identifier), std::move(*block)};
}

ParserResult<Block> Parser::parse_block() {
  std::vector<VarDecl> variable_declarations;
  std::vector<ProcedureDecl> procedure_declarations;

  if (is_current_token(TokenType::VAR)) {
    auto variable_declarations_result = parse_variable_declarations();
    if (!variable_declarations_result) {
      return forward_error(std::move(variable_declarations_result));
    }
    variable_declarations = std::move(*variable_declarations_result);
  }

  if (current_token_->token_type == TokenType::PROCEDURE) {
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

ParserResult<std::vector<VarDecl>> Parser::parse_variable_declarations() {
  if (!is_current_token(TokenType::VAR)) {
    return unexpected_token_error(TokenType::VAR, *current_token_);
  }
  current_token_++;
  std::vector<VarDecl> declarations;
  while (true) {
    auto variable_declaration = parse_variable_declaration();
    if (!variable_declaration) {
      return forward_error(std::move(variable_declaration));
    }
    if (!is_current_token(TokenType::SEMICOLON)) {
      return unexpected_token_error(TokenType::SEMICOLON, *current_token_);
    }
    current_token_++;
    declarations.push_back(std::move(*variable_declaration));

    if (!is_current_token(TokenType::ID)) {
      break;
    }
  }
  return declarations;
}

ParserResult<VarDecl> Parser::parse_variable_declaration() {
  auto variable = parse_variable();
  if (!variable) {
    return forward_error(std::move(variable));
  }

  std::vector<Variable> variables;
  variables.push_back(std::move(*variable));

  while (true) {
    if (!is_current_token(TokenType::COMMA)) {
      break;
    }
    current_token_++;

    auto new_variable = parse_variable();
    if (!new_variable) {
      return forward_error(std::move(new_variable));
    }
    variables.push_back(std::move(*new_variable));
  }

  if (!is_current_token(TokenType::COLON)) {
    return unexpected_token_error(TokenType::COLON, *current_token_);
  }
  current_token_++;

  auto type_spec = parse_type();
  if (!type_spec) {
    return forward_error(std::move(type_spec));
  }
  return VarDecl{node_id_generator.next(), std::move(variables), *type_spec};
}

ParserResult<std::vector<ProcedureDecl>> Parser::parse_procedure_declarations() {
  std::vector<ProcedureDecl> procedure_declarations;
  while (is_current_token(TokenType::PROCEDURE)) {
    auto procedure_declaration = parse_procedure_declaration();
    if (!procedure_declaration) {
      return forward_error(std::move(procedure_declaration));
    }
    procedure_declarations.push_back(std::move(*procedure_declaration));
  }
  return procedure_declarations;
}

ParserResult<ProcedureDecl> Parser::parse_procedure_declaration() {
  if (!is_current_token(TokenType::PROCEDURE)) {
    return unexpected_token_error(TokenType::PROCEDURE, *current_token_);
  }
  current_token_++;

  auto name = parse_identifier();
  if (!name) {
    return forward_error(std::move(name));
  }

  // (LPAREN formal_parameter_list RPAREN)?
  std::vector<Param> params;
  if (is_current_token(TokenType::OPEN_BRACKET)) {
    current_token_++;
    auto params_result = parse_formal_parameter_list();
    if (!params_result) {
      return forward_error(std::move(params_result));
    }
    params = std::move(*params_result);
    if (!is_current_token(TokenType::CLOSED_BRACKET)) {
      return unexpected_token_error(TokenType::CLOSED_BRACKET, *current_token_);
    }
    current_token_++;
  }

  if (!is_current_token(TokenType::SEMICOLON)) {
    return unexpected_token_error(TokenType::SEMICOLON, *current_token_);
  }
  current_token_++;

  auto block = parse_block();
  if (!block) {
    return forward_error(std::move(block));
  }

  if (!is_current_token(TokenType::SEMICOLON)) {
    return unexpected_token_error(TokenType::SEMICOLON, *current_token_);
  }
  current_token_++;
  return ProcedureDecl{node_id_generator.next(), std::move(*name), std::move(params),
                       std::make_shared<Block>(std::move(*block))};
}

ParserResult<std::vector<Param>> Parser::parse_formal_parameter_list() {
  auto formal_parameters = parse_formal_parameters();
  if (!formal_parameters) {
    return forward_error(std::move(formal_parameters));
  }
  std::vector<Param> parameters = std::move(*formal_parameters);

  if (is_current_token(TokenType::SEMICOLON)) {
    current_token_++;
    auto list = parse_formal_parameter_list();
    if (!list) {
      return forward_error(std::move(list));
    }
    for (auto& param : *list) {
      parameters.push_back(std::move(param));
    }
  }

  return std::move(parameters);
}

ParserResult<std::vector<Param>> Parser::parse_formal_parameters() {
  auto id = parse_identifier();
  if (!id) {
    return forward_error(std::move(id));
  }
  std::vector<Identifier> identifiers;
  identifiers.push_back(std::move(*id));

  while (is_current_token(TokenType::COMMA)) {
    current_token_++;
    id = parse_identifier();
    if (!id) {
      return forward_error(std::move(id));
    }
    identifiers.push_back(std::move(*id));
  }

  if (!is_current_token(TokenType::COLON)) {
    return unexpected_token_error(TokenType::COLON, *current_token_);
  }
  current_token_++;

  auto type = parse_type();
  if (!type) {
    return forward_error(std::move(type));
  }

  std::vector<Param> params;
  for (auto& identifier : identifiers) {
    params.push_back(Param{node_id_generator.next(), std::move(identifier), std::move(*type)});
  }
  return params;
}

ParserResult<TokenType> Parser::parse_type() {
  if (!is_current_token(TokenType::INTEGER) && !is_current_token(TokenType::REAL)) {
    return make_error(ParserError{
        fmt::format("Expected type 'INTEGER' or 'REAL', but got token: '{}'", *current_token_)});
  }
  TokenType token_type = current_token_->token_type;
  current_token_++;
  return token_type;
}

ParserResult<CompoundStatement> Parser::parse_compound_statement() {
  if (!is_current_token(TokenType::BEGIN)) {
    return unexpected_token_error(TokenType::BEGIN, *current_token_);
  }
  current_token_++;

  auto statements = parse_statement_list();
  if (!statements) {
    return forward_error(std::move(statements));
  }

  if (!is_current_token(TokenType::END)) {
    return unexpected_token_error(TokenType::END, *current_token_);
  }
  current_token_++;
  return CompoundStatement{node_id_generator.next(), std::move(*statements)};
}

ParserResult<std::vector<Statement>> Parser::parse_statement_list() {
  std::vector<Statement> statements{};

  {
    auto statement = parse_statement();
    if (!statement) {
      return forward_error(std::move(statement));
    }
    statements.push_back((std::move(*statement)));
  }

  while (is_current_token(TokenType::SEMICOLON)) {
    current_token_++;

    auto statement = parse_statement();
    if (!statement) {
      return forward_error(std::move(statement));
    }
    statements.push_back(std::move(*statement));
  }
  return statements;
}

ParserResult<Statement> Parser::parse_statement() {
  if (is_current_token(TokenType::BEGIN)) {
    return parse_compound_statement();
  } else if (is_current_token(TokenType::ID)) {
    // Next element must be at least END_OF_FILE, so it's safe to check its type.
    if ((current_token_ + 1)->token_type == TokenType::OPEN_BRACKET) {
      return parse_procedure_call();
    }

    return parse_assignment_statement();
  } else {
    return parse_empty();
  }
}

ParserResult<AssignmentStatement> Parser::parse_assignment_statement() {
  auto variable = parse_variable();
  if (!variable) {
    return forward_error(std::move(variable));
  }

  if (!is_current_token(TokenType::ASSIGN)) {
    return unexpected_token_error(TokenType::ASSIGN, *current_token_);
  }
  current_token_++;

  auto expr = parse_expr();
  if (!expr) {
    return forward_error(std::move(expr));
  }

  return AssignmentStatement{node_id_generator.next(), std::move(*variable), std::move(*expr)};
}

ParserResult<ProcedureCall> Parser::parse_procedure_call() {
  // proccall_statement : ID LPAREN (expr (COMMA expr)*)? RPAREN
  if (!is_current_token(TokenType::ID)) {
    return unexpected_token_error(TokenType::ID, *current_token_);
  }
  std::string name = *current_token_->value;
  current_token_++;

  if (!is_current_token(TokenType::OPEN_BRACKET)) {
    return unexpected_token_error(TokenType::OPEN_BRACKET, *current_token_);
  }
  current_token_++;

  // Parse arguments.
  std::vector<ExpressionNode> arguments;
  while (true) {
    auto expr = parse_expr();
    if (!expr) {
      return forward_error(std::move(expr));
    }
    arguments.push_back(std::move(*expr));

    if (!is_current_token(TokenType::COMMA)) {
      break;
    }
    current_token_++;
  }

  if (!is_current_token(TokenType::CLOSED_BRACKET)) {
    return unexpected_token_error(TokenType::CLOSED_BRACKET, *current_token_);
  }
  current_token_++;

  return ProcedureCall{node_id_generator.next(), std::move(name), std::move(arguments)};
}

ParserResult<Identifier> Parser::parse_identifier() {
  if (!is_current_token(TokenType::ID)) {
    return unexpected_token_error(TokenType::ID, *current_token_);
  }
  Identifier id = std::move(*current_token_->value);
  current_token_++;
  return id;
}

ParserResult<Variable> Parser::parse_variable() {
  auto identifier = parse_identifier();
  if (!identifier) {
    return forward_error(std::move(identifier));
  }

  return Variable{node_id_generator.next(), std::move(*identifier)};
}

Empty Parser::parse_empty() {
  return Empty{node_id_generator.next()};
}

ParserResult<ExpressionNode> Parser::parse_expr() {
  auto first_term = parse_term();
  if (!first_term) {
    return forward_error(std::move(first_term));
  }
  ExpressionNode result = std::move(*first_term);

  while (is_current_token(TokenType::PLUS) || is_current_token(TokenType::MINUS)) {
    Token op_token = *current_token_;
    current_token_++;

    auto right = parse_term();
    if (!right) {
      return forward_error(std::move(right));
    }

    result = BinOp{node_id_generator.next(),
                   op_token.token_type,
                   std::make_unique<ExpressionNode>(std::move(result)),
                   std::make_unique<ExpressionNode>(std::move(*right))};
  }
  return result;
}

ParserResult<ExpressionNode> Parser::parse_term() {
  auto first_factor = parse_factor();
  if (!first_factor) {
    return forward_error(std::move(first_factor));
  }

  ExpressionNode result = std::move(*first_factor);

  while (is_current_token(TokenType::MUL) || is_current_token(TokenType::INTEGER_DIV)
      || is_current_token(TokenType::REAL_DIV)) {
    auto op_token = *current_token_;
    current_token_++;

    auto right = parse_factor();
    if (!right) {
      return forward_error(std::move(right));
    }

    result = BinOp{node_id_generator.next(),
                   op_token.token_type,
                   std::make_unique<ExpressionNode>(std::move(result)),
                   std::make_unique<ExpressionNode>(std::move(*right))};
  }
  return result;
}

ParserResult<ExpressionNode> Parser::parse_factor() {
  // PLUS factor
  // MINUS factor
  if (is_current_token(TokenType::PLUS) || is_current_token(TokenType::MINUS)) {
    auto op_type = current_token_->token_type;
    current_token_++;

    auto factor = parse_factor();
    if (!factor) {
      return forward_error(std::move(factor));
    }

    return UnaryOp{node_id_generator.next(), op_type, std::make_unique<ExpressionNode>(std::move(*factor))};
  }

  // LPAREN expr RPAREN
  if (is_current_token(TokenType::OPEN_BRACKET)) {
    current_token_++;

    auto expression = parse_expr();
    if (!expression) {
      return forward_error(std::move(expression));
    }

    // Check that the next token is CLOSED_BRACKET
    if (!is_current_token(TokenType::CLOSED_BRACKET)) {
      return unexpected_token_error(TokenType::CLOSED_BRACKET, *current_token_);
    }
    current_token_++;
    return expression;
  }

  // INTEGER_CONST
  // REAL_CONST
  // ID
  if (is_current_token(TokenType::INTEGER_CONST)) {
    auto value = *current_token_->value;
    current_token_++;
    return detail::parse_integer_const(node_id_generator, value);
  } else if (is_current_token(TokenType::REAL_CONST)) {
    auto value = *current_token_->value;
    current_token_++;
    return detail::parse_double_const(node_id_generator, value);
  } else if (is_current_token(TokenType::ID)) {
    return parse_variable();
  } else {
    return make_error(ParserError{fmt::format("Unexpected token {} found while trying to parse factor.\n{}",
                                              current_token_->token_type,
                                              debug_output(text_, current_token_->location))});
  }
}
Error<ParserError> Parser::unexpected_token_error(const TokenType token_type, const Token& actual) {
  return make_error(ParserError{fmt::format("Unexpected token found. Expected: {}. Actual: {}.\n{}",
                                            token_type,
                                            actual,
                                            debug_output(text_, current_token_->location))});
}

bool Parser::is_current_token(TokenType token_type) const {
  return current_token_->token_type == token_type;
}

Parser::Parser(Parser&& parser) noexcept {
  *this = std::move(parser);
}

Parser& Parser::operator=(Parser&& parser) noexcept {
  text_ = std::move(parser.text_);
  tokens_ = std::move(parser.tokens_);
  current_token_ = tokens_.begin();
  node_id_generator = std::move(parser.node_id_generator);
  return *this;
}

}