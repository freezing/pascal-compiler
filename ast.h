//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__AST_H
#define PASCAL_COMPILER_TUTORIAL__AST_H

#include <variant>
#include "token.h"

namespace freezing::interpreter {

using NodeId = int;
struct AstNode;
struct CompoundStatement;
struct AssignmentStatement;
struct BinOp;
struct UnaryOp;
struct Variable;
struct Num;
struct Empty;
struct VarDecl;
struct ProcedureDecl;

// Includes expression, factor and term nodes.
using Identifier = std::string;
using NumType = std::variant<int, double>;
using ExpressionNode = std::variant<BinOp, UnaryOp, Variable, Num>;
using Statement = std::variant<CompoundStatement, AssignmentStatement, Empty>;

struct Empty {
  NodeId id;
};

struct Variable {
  NodeId id;
  Identifier name;
};

struct Num {
  NodeId id;
  NumType value;
};

struct UnaryOp {
  NodeId id;
  // Either PLUS or MINUS.
  TokenType op_type;
  // TODO: Rename to expression.
  std::unique_ptr<ExpressionNode> node;
};

struct BinOp {
  // Either PLUS, MINUS, MUL, INTEGER_DIV, REAL_DIV.
  NodeId id;
  TokenType op_type;
  std::unique_ptr<ExpressionNode> left;
  std::unique_ptr<ExpressionNode> right;
};

struct AssignmentStatement {
  NodeId id;
  Variable variable;
  ExpressionNode expression;
};

struct CompoundStatement {
  NodeId id;
  std::vector<Statement> statements;
};

struct VarDecl {
  NodeId id;
  std::vector<Variable> variables;
  // Either INTEGER or REAL.
  TokenType type_specification;
};

struct Block {
  NodeId id;
  std::vector<VarDecl> variable_declarations;
  std::vector<ProcedureDecl> procedure_declarations;
  CompoundStatement compound_statement;
};

struct ProcedureDecl {
  NodeId id;
  std::string name;
  Block block;
};

struct Program {
  NodeId id;
  Identifier name;
  Block block;
};

}

template<>
struct fmt::formatter<freezing::interpreter::NumType>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::NumType> {
  template<typename FormatContext>
  auto format(const freezing::interpreter::NumType& value, FormatContext& ctx) {
    std::stringstream ss{};
    std::visit([&ss](const auto& v) { ss << v; }, value);
    return fmt::format_to(ctx.out(), "{}", ss.str());
  }
};

#endif //PASCAL_COMPILER_TUTORIAL__AST_H
