//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__PARSER_H
#define PASCAL_COMPILER_TUTORIAL__PARSER_H

#include "lexer.h"
#include "ast.h"

namespace freezing::interpreter {

// Parser that implements the following grammar:
//    program : compound_statement DOT
//
//    compound_statement : BEGIN statement_list END
//
//    statement_list : statement
//                   | statement SEMI statement_list
//
//    statement : compound_statement
//              | assignment_statement
//              | empty
//
//    assignment_statement : variable ASSIGN expr
//
//    empty :
//
//    expr: term ((PLUS | MINUS) term)*
//
//    term: factor ((MUL | DIV) factor)*
//
//    factor : PLUS factor
//           | MINUS factor
//           | INTEGER
//           | LPAREN expr RPAREN
//           | variable
//
//    variable: ID
class Parser {
public:
  explicit Parser(std::string&& text);

  // TODO: Consider returning concrete type rather than AstNode.
  // TODO: Consider having specific AstNode for each grammar entry.
  Result<AstNode> parse_program();
  Result<AstNode> parse_compound_statement();
  Result<AstNode> parse_statement_list();
  Result<AstNode> parse_statement();
  Result<AstNode> parse_assignment_statement();
  AstNode parse_empty();
  Result<AstNode> parse_expr();
  Result<AstNode> parse_term();
  Result<AstNode> parse_factor();
  Result<Variable> parse_variable();

private:
  Lexer lexer_;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__PARSER_H
