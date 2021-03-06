//
// Created by nikola on 3/2/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__AST_H
#define PASCAL_COMPILER_TUTORIAL__AST_H

#include <variant>
#include "token.h"

namespace freezing::interpreter {

struct AstNode;

struct UnaryOp {
  Token op;
  std::unique_ptr<AstNode> node;
};

struct BinOp {
  Token op;
  std::unique_ptr<AstNode> left;
  std::unique_ptr<AstNode> right;
};

struct Num {
  Token token;
};

struct Variable {
  Token token;
};

struct Empty {};

struct AssignmentStatement {
  // TODO: This can technically only be an expression. Revisit storing AstNode in each of these.
  // However, this may be a problem when implementing a visitor? Not a problem, but a tradeoff in some cases. Not sure.
  std::unique_ptr<AstNode> variable;
  std::unique_ptr<AstNode> expression;
};

struct StatementList {
  // TODO: struct Statement { std::unique_ptr<AstNode> } ?
  std::vector<std::unique_ptr<AstNode>> statements;
};

struct CompoundStatement {
  StatementList statements;
};

using NodeValueT = std::variant<CompoundStatement,
                                StatementList,
                                AssignmentStatement,
                                Empty,
                                UnaryOp,
                                BinOp,
                                Num,
                                Variable>;

struct AstNode {
  NodeValueT value;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__AST_H
