//
// Created by nikola on 3/5/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H
#define PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H

#include <variant>
#include "ast.h"

namespace freezing::interpreter {

struct AstVisitorCallbacks {
  std::function<void(const Program&)> program = [](const Program&) {};
  std::function<void(const Block&)> block = [](const Block&) {};
  std::function<void(const VarDecl&)> var_decl = [](const VarDecl&) {};
  std::function<void(const CompoundStatement&)> compound_statement = [](const CompoundStatement&) {};
  std::function<void(const AssignmentStatement&)> assignment_statement = [](const AssignmentStatement&) {};
  std::function<void(const UnaryOp&)> unary_op = [](const UnaryOp&) {};
  std::function<void(const BinOp&)> bin_op = [](const BinOp&) {};
  std::function<void(const Variable&)> variable = [](const Variable&) {};
  std::function<void(const Num&)> num = [](const Num&) {};
};

struct AstVisitorFn {
  struct ExpressionVisitorFn {
    const AstVisitorFn& self;

    void operator()(const BinOp& bin_op) const {
      self.visit(bin_op);
    }

    void operator()(const UnaryOp& unary_op) const {
      self.visit(unary_op);
    }

    void operator()(const Variable& variable) const {
      self.visit(variable);
    }

    void operator()(const Num& num) const {
      self.visit(num);
    }
  };

  struct StatementVisitorFn {
    const AstVisitorFn& self;

    void operator()(const CompoundStatement& compound_statement) const {
      self.visit(compound_statement);
    }

    void operator()(const AssignmentStatement& assignment_statement) const {
      self.visit(assignment_statement);
    }

    void operator()(const Empty&) const {}
  };

  AstVisitorCallbacks callbacks;

  void visit(const Program& program) const {
    visit(program.block);
    std::invoke(callbacks.program, program);
  }

  void visit(const Block& block) const {
    for (const auto& var_decl : block.declarations) {
      visit(var_decl);
    }
    visit(block.compound_statement);
    std::invoke(callbacks.block, block);
  }

  void visit(const VarDecl& var_decl) const {
    std::invoke(callbacks.var_decl, var_decl);
  }

  void visit(const CompoundStatement& compound_statement) const {
    for (const auto& statement : compound_statement.statements) {
      std::visit(StatementVisitorFn{*this}, statement);
    }
    std::invoke(callbacks.compound_statement, compound_statement);
  }

  void visit(const AssignmentStatement& assignment_statement) const {
    visit(assignment_statement.variable);
    std::visit(ExpressionVisitorFn{*this}, assignment_statement.expression);
    std::invoke(callbacks.assignment_statement, assignment_statement);
  }

  void visit(const BinOp& bin_op) const {
    std::visit(ExpressionVisitorFn{*this}, *bin_op.left);
    std::visit(ExpressionVisitorFn{*this}, *bin_op.right);
    std::invoke(callbacks.bin_op, bin_op);
  }

  void visit(const UnaryOp& unary_op) const {
    std::visit(ExpressionVisitorFn{*this}, *unary_op.node);
    std::invoke(callbacks.unary_op, unary_op);
  }

  void visit(const Variable& variable) const {
    std::invoke(callbacks.variable, variable);
  }

  void visit(const Num& num) const {
    std::invoke(callbacks.num, num);
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H
