//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL_LEXER_H
#define PASCAL_COMPILER_TUTORIAL_LEXER_H

#include <cassert>
#include <fmt/format.h>
#include <map>
#include "result.h"
#include "token.h"

namespace freezing::interpreter {

struct LexerError {
  CharLocation location;
  std::string message;

  friend std::ostream& operator<<(std::ostream& os, const LexerError& e) {
    return os << fmt::format("LexerError(line and column = ({}, {})): {}",
                             e.location.line_number,
                             e.location.column_number,
                             e.message);
  }
};

template<typename T>
using LexerResult = Result<T, LexerError>;

class Lexer {
public:
  explicit Lexer(std::string&& text);
  explicit Lexer(const std::string& text);

  // Returns the token after the last successful advance() call.
  // The result is undefined if the advance() method hasn't been called.
  const Token& peek();
  LexerResult<Void> advance();

private:
  // Text to interpret.
  std::string text_;
  // Position of the next character in text.
  int pos_;
  Token current_token_;
  CharLocation current_location_;

  char peek_char() const;
  char next_char();
  void skip_whitespaces();
  void skip_until_comment_close();
  LexerResult<Token> parse_token();
};

}

#endif //PASCAL_COMPILER_TUTORIAL_LEXER_H
