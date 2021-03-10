//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__PARSER_H
#define PASCAL_COMPILER_TUTORIAL__PARSER_H

#include "lexer.h"
#include "ast.h"

namespace freezing::interpreter {

// Parser that implements the following grammar:
//
//    program : PROGRAM variable SEMI block DOT
//
//    block : declarations compound_statement
//
//    declarations : VAR (variable_declaration SEMI)+
//                 | empty
//
//    variable_declaration : ID (COMMA ID)* COLON type_spec
//
//    type_spec : INTEGER | REAL
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
//    expr : term ((PLUS | MINUS) term)*
//
//    term : factor ((MUL | INTEGER_DIV | FLOAT_DIV) factor)*
//
//    factor : PLUS factor
//           | MINUS factor
//           | INTEGER_CONST
//           | REAL_CONST
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
  Result<Block> parse_block();
  Result<std::vector<VarDecl>> parse_declarations();
  Result<VarDecl> parse_variable_declaration();
  Result<Type> parse_type();
  Result<AstNode> parse_compound_statement();
  Result<AstNode> parse_statement_list();
  Result<AstNode> parse_statement();
  Result<AstNode> parse_assignment_statement();
  AstNode parse_empty();
  Result<AstNode> parse_expr();
  Result<AstNode> parse_term();
  Result<AstNode> parse_factor();
  // TODO: Rename Variable to Identifier.
  Result<Variable> parse_variable();

private:
  Lexer lexer_;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__PARSER_H
