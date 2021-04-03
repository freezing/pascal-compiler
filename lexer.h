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

  LexerResult<Token> pop();
  LexerResult<Token> pop(TokenType token_type);
  // TODO: peek should always return Token.
  // This can be achieved by moving check in the advance.
  const LexerResult<Token>& peek();
  LexerResult<Void> advance(TokenType token_type);
  void advance();

private:
  // Text to interpret.
  std::string text_;
  // Position of the next character in text.
  int pos_;
  LexerResult<Token> next_token_;
  CharLocation current_location_;

  char peek_char() const;
  char next_char();
  void skip_whitespaces();
  void skip_until_comment_close();
  LexerResult<Token> parse_token();
};

}

#endif //PASCAL_COMPILER_TUTORIAL_LEXER_H
