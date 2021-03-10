//
// Created by nikola on 3/2/2021.
//

#include "lexer.h"

namespace freezing::interpreter {

const std::map<std::string, Token>
    kReservedKeywords{
    {"PROGRAM", Token{TokenType::PROGRAM, "PROGRAM"}},
    {"VAR", Token{TokenType::VAR, "VAR"}},
    {"DIV", Token{TokenType::INTEGER_DIV, "DIV"}},
    {"INTEGER", Token{TokenType::INTEGER, "INTEGER"}},
    {"REAL", Token{TokenType::REAL, "REAL"}},
    {"BEGIN", Token{TokenType::BEGIN, "BEGIN"}},
    {"END", Token{TokenType::END, "END"}},
};

Lexer::Lexer(std::string&& text) : text_{std::move(text)}, pos_{0} {
  next_token_ = parse_token();
}

Result<Token> Lexer::pop() {
  auto ret = std::move(next_token_);
  advance();
  return ret;
}

Result<Token> Lexer::pop(TokenType token_type) {
  auto ret = std::move(next_token_);
  auto check = advance(token_type);
  if (!check) {
    return forward_error(std::move(check));
  }
  return ret;
}

const Result<Token>& Lexer::peek() {
  return next_token_;
}

Result<Void> Lexer::advance(TokenType token_type) {
  auto check_token_type = [token_type](const Token& token) -> Result<Void> {
    if (token.token_type != token_type) {
      return make_error(fmt::format("Unexpected token type: {}", token_type));
    }
    return {};
  };
  return next_token_.and_then(check_token_type).map([this](auto&&) {
    advance();
    return Void{};
  });
}

void Lexer::advance() {
  next_token_ = parse_token();
}

char Lexer::peek_char() const {
  return text_[pos_];
}
char Lexer::next_char() {
  assert(!isspace(text_[pos_]));
  return text_[pos_++];
}
void Lexer::skip_whitespaces() {
  while (pos_ < text_.size() && isspace(text_[pos_])) {
    pos_++;
  }
}

void Lexer::skip_until_comment_close() {
  while (pos_ < text_.size()) {
    if (next_char() == '}') {
      break;
    }
  }
}

Result<Token> Lexer::parse_token() {
  skip_whitespaces();

  if (pos_ >= text_.size()) {
    return {Token{TokenType::END_OF_FILE, {}}};
  }

  // TODO: Use peek_char().
  char current_char = next_char();

  if (current_char == '{') {
    skip_until_comment_close();
  } else if (isdigit(current_char)) {
    std::string value;
    value += current_char;

    while (pos_ < text_.size() && isdigit(peek_char())) {
      value += next_char();
    }

    if (peek_char() != '.') {
      return Token{TokenType::INTEGER_CONST, std::move(value)};
    }
    // Otherwise it's REAL.
    value += next_char();
    while (pos_ < text_.size() && isdigit(peek_char())) {
      value += next_char();
    }
    return Token{TokenType::REAL_CONST, std::move(value)};
  } else if (current_char == '+') {
    return Token{TokenType::PLUS, "+"};
  } else if (current_char == '-') {
    return Token{TokenType::MINUS, "-"};
  } else if (current_char == '*') {
    return Token{TokenType::MUL, "*"};
  } else if (current_char == '/') {
    return Token{TokenType::REAL_DIV, "/"};
  } else if (current_char == '(') {
    return Token{TokenType::OPEN_BRACKET, "("};
  } else if (current_char == ')') {
    return Token{TokenType::CLOSED_BRACKET, ")"};
  } else if (current_char == '.') {
    return Token{TokenType::DOT, "."};
  } else if (current_char == ':' && peek_char() == '=') {
    next_char();
    return Token{TokenType::ASSIGN, ":="};
  } else if (current_char == ':') {
    return Token{TokenType::COLON, ":"};
  } else if (current_char == ';') {
    return Token{TokenType::SEMICOLON, ";"};
  } else if (current_char == ',') {
    return Token{TokenType::COMMA, ","};
  } else if (isalpha(current_char)) {
    std::string id;
    id += current_char;

    while (pos_ < text_.size() && isalnum(peek_char())) {
      id += next_char();
    }

    auto it = kReservedKeywords.find(id);
    if (it == kReservedKeywords.end()) {
      return Token{TokenType::ID, std::move(id)};
    }
    return it->second;
  }
  return make_error(fmt::format("Unknown character: {}", current_char));
}

}