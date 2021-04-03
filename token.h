//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL_TOKEN_H
#define PASCAL_COMPILER_TUTORIAL_TOKEN_H

#include <cassert>
#include <fmt/format.h>
#include <optional>
#include "optional_formatter.h"
#include "stringstream_formatter.h"

namespace freezing::interpreter {

enum class TokenType {
  END_OF_FILE,
  INTEGER_CONST,
  REAL_CONST,
  PLUS,
  MINUS,
  MUL,
  INTEGER_DIV,
  // TODO: Rename to LPAREN and RPAREN
  OPEN_BRACKET,
  CLOSED_BRACKET,
  DOT,
  BEGIN,
  END,
  ASSIGN,
  COLON,
  SEMICOLON,
  ID,
  PROGRAM,
  VAR,
  REAL,
  INTEGER,
  REAL_DIV,
  COMMA,
  PROCEDURE,
};

inline std::ostream& operator<<(std::ostream& os, const TokenType& token_type) {
  std::string type_string = "UNKNOWN";
  switch (token_type) {
  case TokenType::END_OF_FILE:
    type_string = "END_OF_FILE";
    break;
  case TokenType::INTEGER_CONST:
    type_string = "INTEGER_CONST";
    break;
  case TokenType::PLUS:
    type_string = "PLUS";
    break;
  case TokenType::MINUS:
    type_string = "MINUS";
    break;
  case TokenType::MUL:
    type_string = "MUL";
    break;
  case TokenType::INTEGER_DIV:
    type_string = "INTEGER_DIV";
    break;
  case TokenType::OPEN_BRACKET:
    type_string = "OPEN_BRACKET";
    break;
  case TokenType::CLOSED_BRACKET:
    type_string = "CLOSED_BRACKET";
    break;
  case TokenType::DOT:
    type_string = "DOT";
    break;
  case TokenType::BEGIN:
    type_string = "BEGIN";
    break;
  case TokenType::END:
    type_string = "END";
    break;
  case TokenType::ASSIGN:
    type_string = "ASSIGN";
    break;
  case TokenType::SEMICOLON:
    type_string = "SEMICOLON";
    break;
  case TokenType::ID:
    type_string = "ID";
    break;
  case TokenType::PROGRAM:
    type_string = "PROGRAM";
    break;
  case TokenType::VAR:
    type_string = "VAR";
    break;
  case TokenType::REAL:
    type_string = "REAL";
    break;
  case TokenType::REAL_DIV:
    type_string = "REAL_DIV";
    break;
  case TokenType::REAL_CONST:
    type_string = "REAL_CONST";
    break;
  case TokenType::INTEGER:
    type_string = "INTEGER";
    break;
  case TokenType::COMMA:
    type_string = "COMMA";
    break;
  case TokenType::COLON:
    type_string = "COLON";
    break;
  case TokenType::PROCEDURE:
    type_string = "PROCEDURE";
    break;
  }
  return os << type_string;
}

struct CharLocation {
  int line_number;
  int column_number;
};

struct Token {
  TokenType token_type;
  std::optional<std::string> value;
  // Location of the first character of the token.
  CharLocation location;

  int AsInt() const {
    assert(value.has_value());
    return ::atoi(value->c_str());
  }

  friend std::ostream& operator<<(std::ostream& os, const Token& token) {
    // TODO: Use fmt::formatter.
    return os << "Token(" << token.token_type << ", lexeme=" << token.value << ")";
  }
};

}

template<>
struct fmt::formatter<freezing::interpreter::Token>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::Token> {
};

template<>
struct fmt::formatter<freezing::interpreter::TokenType>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::TokenType> {
};

#endif //PASCAL_COMPILER_TUTORIAL_TOKEN_H
