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

class Lexer {
public:
  explicit Lexer(std::string&& text);

  Result<Token> pop();
  Result<Token> pop(TokenType token_type);
  // TODO: peek should always return Token.
  // This can be achieved by moving check in the advance.
  const Result<Token>& peek();
  Result<Void> advance(TokenType token_type);
  void advance();

private:
  // Text to interpret.
  std::string text_;
  // Position of the next character in text.
  int pos_;
  Result<Token> next_token_;

  char peek_char() const;
  char next_char();
  void skip_whitespaces();
  void skip_until_comment_close();
  Result<Token> parse_token();
};

}

#endif //PASCAL_COMPILER_TUTORIAL_LEXER_H
