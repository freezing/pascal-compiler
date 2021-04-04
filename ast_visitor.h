//
// Created by nikola on 3/5/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H
#define PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H

#include <variant>
#include "ast.h"

namespace freezing::interpreter {

struct NodeIdExtractorFn {
  template<typename T>
  NodeId operator()(const T& node) const {
    return node.id;
  }
};

static NodeId node_id(const Statement& statement) {
  return std::visit(NodeIdExtractorFn{}, statement);
}

static NodeId node_id(const ExpressionNode& expression) {
  return std::visit(NodeIdExtractorFn{}, expression);
}

struct AstVisitorCallbacks {
  std::function<void(const Program&)> program_pre = [](const Program&) {};
  std::function<void(const Program&)> program_post = [](const Program&) {};
  std::function<void(const Block&)> block = [](const Block&) {};
  std::function<void(const VarDecl&)> var_decl_pre = [](const VarDecl&) {};
  std::function<void(const VarDecl&)> var_decl_post = [](const VarDecl&) {};
  std::function<void(const ProcedureDecl&)> procedure_decl_pre = [](const ProcedureDecl&) {};
  std::function<void(const ProcedureDecl&)> procedure_decl_post = [](const ProcedureDecl&) {};
  std::function<void(const Param&)> param = [](const Param&) {};
  std::function<void(const CompoundStatement&)> compound_statement = [](const CompoundStatement&) {};
  std::function<void(const AssignmentStatement&)> assignment_statement = [](const AssignmentStatement&) {};
  std::function<void(const ProcedureCall&)> procedure_call_post = [](const ProcedureCall&) {};
  std::function<void(const UnaryOp&)> unary_op = [](const UnaryOp&) {};
  std::function<void(const BinOp&)> bin_op = [](const BinOp&) {};
  std::function<void(const Variable&)> variable = [](const Variable&) {};
  std::function<void(const Num&)> num = [](const Num&) {};
  std::function<void(const Empty&)> empty = [](const Empty&) {};
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

    void operator()(const ProcedureCall& procedure_call) const {
      self.visit(procedure_call);
    }

    void operator()(const Empty& empty) const {
      self.visit(empty);
    }
  };

  AstVisitorCallbacks callbacks;

  void visit(const Program& program) const {
    std::invoke(callbacks.program_pre, program);
    visit(program.block);
    std::invoke(callbacks.program_post, program);
  }

  void visit(const Block& block) const {
    for (const auto& var_decl : block.variable_declarations) {
      visit(var_decl);
    }
    for (const auto& procedure_decl : block.procedure_declarations) {
      visit(procedure_decl);
    }
    visit(block.compound_statement);
    std::invoke(callbacks.block, block);
  }

  void visit(const VarDecl& var_decl) const {
    std::invoke(callbacks.var_decl_pre, var_decl);
    for (const auto& variable : var_decl.variables) {
      visit(variable);
    }
    std::invoke(callbacks.var_decl_post, var_decl);
  }

  void visit(const ProcedureDecl& procedure_decl) const {
    std::invoke(callbacks.procedure_decl_pre, procedure_decl);
    for (const auto& param : procedure_decl.parameters) {
      visit(param);
    }
    visit(procedure_decl.block);
    std::invoke(callbacks.procedure_decl_post, procedure_decl);
  }

  void visit(const Param& param) const {
    std::invoke(callbacks.param, param);
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

  void visit(const ProcedureCall& procedure_call) const {
    for (const auto& expr : procedure_call.parameters) {
      std::visit(ExpressionVisitorFn{*this}, expr);
    }
    std::invoke(callbacks.procedure_call_post, procedure_call);
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

  void visit(const Empty& empty) const {
    std::invoke(callbacks.empty, empty);
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H
