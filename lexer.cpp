//
// Created by nikola on 3/2/2021.
//

#include <iomanip>
#include <utility>
#include "lexer.h"
#include "debug.h"

namespace freezing::interpreter {

// TODO: Should be token types.
const std::map<std::string, TokenType>
    kReservedKeywords{
    {"PROGRAM", TokenType::PROGRAM},
    {"VAR", TokenType::VAR},
    {"DIV", TokenType::INTEGER_DIV},
    {"INTEGER", TokenType::INTEGER},
    {"REAL", TokenType::REAL},
    {"BEGIN", TokenType::BEGIN},
    {"END", TokenType::END},
    {"PROCEDURE", TokenType::PROCEDURE},
};

Lexer::Lexer(std::string&& text) : text_{std::move(text)}, pos_{0}, current_location_{0, 0} {}

Lexer::Lexer(const std::string& text) : text_{text}, pos_{0}, current_location_{0, 0} {}

const Token& Lexer::peek() {
  return current_token_;
}

LexerResult<Void> Lexer::advance() {
  LexerResult<Token> result = parse_token();
  if (!result) {
    return forward_error(std::move(result));
  }
  current_token_ = std::move(*result);
  return {};
}

char Lexer::peek_char() const {
  return text_[pos_];
}

char Lexer::next_char() {
  current_location_.column_number++;
  if (text_[pos_] == '\n') {
    current_location_.line_number++;
    current_location_.column_number = 0;
  }
  return text_[pos_++];
}

void Lexer::skip_whitespaces() {
  while (pos_ < text_.size() && isspace(text_[pos_])) {
    next_char();
  }
}

void Lexer::skip_until_comment_close() {
  while (pos_ < text_.size()) {
    if (next_char() == '}') {
      break;
    }
  }
}

LexerResult<Token> Lexer::parse_token() {
  while (true) {
    skip_whitespaces();
    if (pos_ >= text_.size()) {
      return {Token{current_location_, TokenType::END_OF_FILE, {}}};
    }

    // TODO: Use peek_char().
    char current_char = next_char();
    CharLocation location = current_location_;

    if (current_char == '{') {
      skip_until_comment_close();
      continue;
    } else if (isdigit(current_char)) {
      std::string value;
      value += current_char;

      while (pos_ < text_.size() && isdigit(peek_char())) {
        value += next_char();
      }

      if (peek_char() != '.') {
        return Token{location, TokenType::INTEGER_CONST, std::move(value)};
      }
      // Otherwise it's REAL.
      value += next_char();
      while (pos_ < text_.size() && isdigit(peek_char())) {
        value += next_char();
      }
      return Token{location, TokenType::REAL_CONST, std::move(value)};
    } else if (current_char == '+') {
      return Token{location, TokenType::PLUS, "+"};
    } else if (current_char == '-') {
      return Token{location, TokenType::MINUS, "-"};
    } else if (current_char == '*') {
      return Token{location, TokenType::MUL, "*"};
    } else if (current_char == '/') {
      return Token{location, TokenType::REAL_DIV, "/"};
    } else if (current_char == '(') {
      return Token{location, TokenType::OPEN_BRACKET, "("};
    } else if (current_char == ')') {
      return Token{location, TokenType::CLOSED_BRACKET, ")"};
    } else if (current_char == '.') {
      return Token{location, TokenType::DOT, "."};
    } else if (current_char == ':' && peek_char() == '=') {
      next_char();
      return Token{location, TokenType::ASSIGN, ":="};
    } else if (current_char == ':') {
      return Token{location, TokenType::COLON, ":"};
    } else if (current_char == ';') {
      return Token{location, TokenType::SEMICOLON, ";"};
    } else if (current_char == ',') {
      return Token{location, TokenType::COMMA, ","};
    } else if (isalpha(current_char)) {
      std::string id;
      id += current_char;

      while (pos_ < text_.size() && isalnum(peek_char())) {
        id += next_char();
      }

      auto it = kReservedKeywords.find(id);
      if (it == kReservedKeywords.end()) {
        return Token{location, TokenType::ID, std::move(id)};
      }
      return Token{location, it->second, fmt::format("{}", it->second)};
    }
    return make_error(LexerError{location, fmt::format("Unknown character sequence: {}\n{}",
                                                       current_char,
                                                       debug_output(text_, current_location_))});
  }
}

}