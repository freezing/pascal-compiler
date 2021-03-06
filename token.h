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
  END_OF_FILE = 0,
  INTEGER = 1,
  PLUS = 2,
  MINUS = 3,
  MUL = 4,
  DIV = 5,
  OPEN_BRACKET = 6,
  CLOSED_BRACKET = 7,
  DOT = 8,
  BEGIN = 9,
  END = 10,
  ASSIGN = 11,
  SEMICOLON = 12,
  ID = 13,
};

inline std::ostream& operator<<(std::ostream& os, const TokenType& token_type) {
  std::string type_string = "UNKNOWN";
  switch (token_type) {
  case TokenType::END_OF_FILE:
    type_string = "END_OF_FILE";
    break;
  case TokenType::INTEGER:
    type_string = "INTEGER";
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
  case TokenType::DIV:
    type_string = "DIV";
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
  }
  return os << type_string;
}

struct Token {
  TokenType token_type;
  std::optional<std::string> value;

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
