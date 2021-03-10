//
// Created by nikola on 3/5/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H
#define PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H

#include <variant>
#include "ast.h"

namespace freezing::interpreter {

namespace detail {

struct AstNodeVariantVisitTypedFn {

};

}

template<typename NodeResultT>
struct AstVisitorFn {
  std::function<NodeResultT(const AstNode*, const CompoundStatement&, std::vector<NodeResultT>&&)> compound_statement_callback;
  std::function<NodeResultT(const AstNode*, const StatementList&, std::vector<NodeResultT>&&)> statement_list_callback;
  std::function<NodeResultT(const AstNode*, const AssignmentStatement&, NodeResultT&&, NodeResultT&&)>
      assignment_statement_callback;
  std::function<NodeResultT(const AstNode*)> empty_callback;
  std::function<NodeResultT(const AstNode*, const UnaryOp&, NodeResultT&&)> unary_op_callback;
  std::function<NodeResultT(const AstNode*, const BinOp&, NodeResultT&&, NodeResultT&&)>
      bin_op_callback;
  std::function<NodeResultT(const AstNode*, const Num&)> num_callback;
  std::function<NodeResultT(const AstNode*, const Variable&)> variable_callback;

  NodeResultT Visit(const AstNode* tree) {
    assert(tree != nullptr);

    struct VisitorWithResult {
      AstVisitorFn& self;
      const AstNode* ast_node;

      NodeResultT operator()(const CompoundStatement& compound_statement) {
        std::vector<NodeResultT> results;
        for (const auto& child : compound_statement.statements.statements) {
          results.push_back(self.Visit(child.get()));
        }
        return std::invoke(self.compound_statement_callback, ast_node, compound_statement, std::move(results));
      }

      NodeResultT operator()(const StatementList& statement_list) {
        std::vector<NodeResultT> results;
        for (const auto& child : statement_list.statements) {
          results.push_back(self.Visit(child.get()));
        }
        return std::invoke(self.statement_list_callback, ast_node, statement_list, std::move(results));
      }

      NodeResultT operator()(const AssignmentStatement& assignment_statement) {
        auto variable_result = self.Visit(assignment_statement.variable.get());
        auto expression_result = self.Visit(assignment_statement.expression.get());
        return std::invoke(self.assignment_statement_callback,
                           ast_node,
                           assignment_statement,
                           std::move(variable_result),
                           std::move(expression_result));
      }

      NodeResultT operator()(const Empty&) {
        return std::invoke(self.empty_callback, ast_node);
      }

      NodeResultT operator()(const UnaryOp& unary_op) {
        auto node_result = self.Visit(unary_op.node.get());
        return std::invoke(self.unary_op_callback, ast_node, unary_op, std::move(node_result));
      }

      NodeResultT operator()(const BinOp& bin_op) {
        auto left_result = self.Visit(bin_op.left.get());
        auto right_result = self.Visit(bin_op.right.get());
        return std::invoke(
            self.bin_op_callback, ast_node, bin_op, std::move(left_result), std::move(right_result));
      }

      NodeResultT operator()(const Num& num) {
        return std::invoke(self.num_callback, ast_node, num);
      }

      NodeResultT operator()(const Variable& variable) {
        return std::invoke(self.variable_callback, ast_node, variable);
      }
    };

    return std::visit(VisitorWithResult{*this, tree}, tree->value);
  }
};

template<>
struct AstVisitorFn<void> {
  // TODO: void* should be ID.
  std::function<void(const void*, const Program&)> program_callback;
  std::function<void(const void*, const Block&)> block_callback;
  std::function<void(const void*, const VarDecl&)> var_decl_callback;
  std::function<void(const void*, const Type&)> type_callback;
  std::function<void(const void*, const CompoundStatement&)> compound_statement_callback;
  std::function<void(const void*, const StatementList&)> statement_list_callback;
  std::function<void(const void*, const AssignmentStatement&)> assignment_statement_callback;
  std::function<void(const void*)> empty_callback;
  std::function<void(const void*, const UnaryOp&)> unary_op_callback;
  std::function<void(const void*, const BinOp&)> bin_op_callback;
  std::function<void(const void*, const Num&)> num_callback;
  std::function<void(const void*, const Variable&)> variable_callback;

  // TODO: Change to const AstNode& tree
  void Visit(const AstNode* tree) {
    assert(tree != nullptr);

    struct VisitorVoid {
      AstVisitorFn& self;
      const AstNode* ast_node;

      void operator()(const Program& program) {
        self.Visit((void*)&program)
      }

      void operator()(const CompoundStatement& compound_statement) {
        for (const auto& child : compound_statement.statements.statements) {
          self.Visit(child.get());
        }
        std::invoke(self.compound_statement_callback, ast_node, compound_statement);
      }

      void operator()(const StatementList& statement_list) {
        for (const auto& child : statement_list.statements) {
          self.Visit(child.get());
        }
        std::invoke(self.statement_list_callback, ast_node, statement_list);
      }

      void operator()(const AssignmentStatement& assignment_statement) {
        self.Visit(assignment_statement.variable.get());
        self.Visit(assignment_statement.expression.get());
        std::invoke(self.assignment_statement_callback, ast_node, assignment_statement);
      }

      void operator()(const Empty&) {
        std::invoke(self.empty_callback, ast_node);
      }

      void operator()(const UnaryOp& unary_op) {
        self.Visit(unary_op.node.get());
        std::invoke(self.unary_op_callback, ast_node, unary_op);
      }

      void operator()(const BinOp& bin_op) {
        self.Visit(bin_op.left.get());
        self.Visit(bin_op.right.get());
        std::invoke(self.bin_op_callback, ast_node, bin_op);
      }

      void operator()(const Num& num) { std::invoke(self.num_callback, ast_node, num); }

      void operator()(const Variable& variable) {
        std::invoke(self.variable_callback, ast_node, variable);
      }
    };

    std::visit(VisitorVoid{*this, tree}, tree->value);
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__AST_VISITOR_H
