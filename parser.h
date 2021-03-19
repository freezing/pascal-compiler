//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__PARSER_H
#define PASCAL_COMPILER_TUTORIAL__PARSER_H

#include "lexer.h"
#include "ast.h"
#include "id_generator.h"

namespace freezing::interpreter {

// Parser that implements the following grammar:
//
//    program : PROGRAM variable SEMI block DOT
//
//    block : variable_declarations procedure_declarations compound_statement
//
//    variable_declarations : VAR (variable_declaration SEMI)+
//                 | empty
//
//    procedure_declarations: (PROCEDURE ID SEMI block SEMI)*
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

  Result<Program> parse_program();
  Result<Block> parse_block();
  Result<std::vector<VarDecl>> parse_variable_declarations();
  Result<VarDecl> parse_variable_declaration();
  Result<std::vector<ProcedureDecl>> parse_procedure_declarations();
  // Either INTEGER or REAL.
  Result<TokenType> parse_type();
  Result<CompoundStatement> parse_compound_statement();
  Result<std::vector<Statement>> parse_statement_list();
  Result<Statement> parse_statement();
  Result<AssignmentStatement> parse_assignment_statement();
  Empty parse_empty();
  Result<ExpressionNode> parse_expr();
  Result<ExpressionNode> parse_term();
  Result<ExpressionNode> parse_factor();
  Result<Identifier> parse_identifier();
  Result<Variable> parse_variable();

private:
  Lexer lexer_;
  IdGenerator<NodeId> node_id_generator;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__PARSER_H
