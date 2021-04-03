//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__PARSER_H
#define PASCAL_COMPILER_TUTORIAL__PARSER_H

#include <iostream>
#include "lexer.h"
#include "ast.h"
#include "id_generator.h"

namespace freezing::interpreter {

struct ParserError {
  std::string message;

  friend std::ostream& operator<<(std::ostream& os, const ParserError& e) {
    return os << "ParserError: " << e.message;
  }
};

template<typename T>
using ParserResult = Result<T, std::variant<ParserError, LexerError>>;

// Parser that implements the following grammar:
//
//    program_post : PROGRAM variable SEMI block DOT
//
//    block : variable_declarations procedure_declarations compound_statement
//
//    variable_declarations : VAR (variable_declaration SEMI)+
//                          | empty
//
//    procedure_declarations: (PROCEDURE ID (LPAREN formal_parameter_list RPAREN)? SEMI block SEMI)*
//
//    formal_parameter_list : formal_parameters
//                          | formal_parameters SEMI formal_parameter_list
//
//    formal_parameters : ID (COMMA ID)* COLON type_spec
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

  ParserResult<Program> parse_program();
  ParserResult<Block> parse_block();
  ParserResult<std::vector<VarDecl>> parse_variable_declarations();
  ParserResult<VarDecl> parse_variable_declaration();
  ParserResult<std::vector<ProcedureDecl>> parse_procedure_declarations();
  ParserResult<ProcedureDecl> parse_procedure_declaration();
  ParserResult<std::vector<Param>> parse_formal_parameter_list();
  ParserResult<std::vector<Param>> parse_formal_parameters();
  // Either INTEGER or REAL.
  ParserResult<TokenType> parse_type();
  ParserResult<CompoundStatement> parse_compound_statement();
  ParserResult<std::vector<Statement>> parse_statement_list();
  ParserResult<Statement> parse_statement();
  ParserResult<AssignmentStatement> parse_assignment_statement();
  Empty parse_empty();
  ParserResult<ExpressionNode> parse_expr();
  ParserResult<ExpressionNode> parse_term();
  ParserResult<ExpressionNode> parse_factor();
  ParserResult<Identifier> parse_identifier();
  ParserResult<Variable> parse_variable();

private:
  Lexer lexer_;
  IdGenerator<NodeId> node_id_generator;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__PARSER_H
