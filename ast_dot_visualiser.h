//
// Created by nikola on 3/2/2021.
//

#include "ast.h"
#include "ast_visitor.h"

#ifndef PASCAL_COMPILER_TUTORIAL__AST_DOT_VISUALISER_H
#define PASCAL_COMPILER_TUTORIAL__AST_DOT_VISUALISER_H

namespace freezing::interpreter {

class AstDotVisualiser {
public:
  explicit AstDotVisualiser(const AstNode* tree) : tree_{tree} {}

  std::string generate() {
    std::string body{};

    auto compound_statement_callback =
        [&body](const AstNode* node, const CompoundStatement& compound_statement) -> void {
          body += fmt::format(
              R"(  node{} [label="{}"]
)",
              reinterpret_cast<const int64_t>(node),
              "CompoundStatement");
          for (const auto& statement : compound_statement.statements.statements) {
            body += node_edge(node, statement.get());
          }
        };

    auto statement_list_callback = [&body](const AstNode* node, const StatementList& statement_list) -> void {
      body += fmt::format(
          R"(  node{} [label="{}"]
)",
          reinterpret_cast<const int64_t>(node),
          "StatementList");
      for (const auto& statement : statement_list.statements) {
        body += node_edge(node, statement.get());
      }
    };

    auto assignment_statement_callback =
        [&body](const AstNode* node, const AssignmentStatement& assignment_statement) -> void {
          body += fmt::format(
              R"(  node{} [label="{}"]
)",
              reinterpret_cast<const int64_t>(node),
              "Assignment");
          body += node_edge(node, assignment_statement.variable.get());
          body += node_edge(node, assignment_statement.expression.get());
        };

    auto empty_callback = [&body](const AstNode* node) -> void {
      body += fmt::format(
          R"(  node{} [label="{}"]
)",
          reinterpret_cast<const int64_t>(node),
          "Empty");
    };

    auto unary_op_callback = [&body](const AstNode* node, const UnaryOp& unary_op) -> void {
      body += fmt::format(
          R"(  node{} [label="{}"]
)",
          reinterpret_cast<const int64_t>(node),
          unary_op.op);
      body += node_edge(node, unary_op.node.get());
    };

    auto bin_op_callback = [&body](const AstNode* node, const BinOp& bin_op) -> void {
      body += fmt::format(
          R"(  node{} [label="{}"]
)",
          reinterpret_cast<const int64_t>(node),
          bin_op.op);
      body += node_edge(node, bin_op.left.get());
      body += node_edge(node, bin_op.right.get());
    };

    auto num_callback = [&body](const AstNode* node, const Num& num) -> void {
      body += fmt::format(
          R"(  node{} [label="{}"]
)",
          reinterpret_cast<const int64_t>(node),
          num.token);
    };

    auto variable_callback = [&body](const AstNode* node, const Variable& variable) -> void {
      body += fmt::format(
          R"(  node{} [label="{}"]
)",
          reinterpret_cast<const int64_t>(node),
          variable.token);
    };

    AstVisitorFn<void>{compound_statement_callback, statement_list_callback, assignment_statement_callback,
                       empty_callback, unary_op_callback, bin_op_callback, num_callback, variable_callback}
        .Visit(tree_);

    const std::string header =
        R"(digraph astgraph {
           node [shape=circle, fontsize=12, fontname="Courier", height=.1];
           ranksep=.3;
           edge [arrowsize=.5]
)";
    const std::string footer = "}";
    return header + body + footer;
  }

private:
  const AstNode* tree_;

  static std::string node_edge(const AstNode* parent, const AstNode* child) {
    assert(parent != nullptr && child != nullptr);
    return fmt::format(
        R"(  node{} -> node{}
)",
        reinterpret_cast<const int64_t>(parent),
        reinterpret_cast<const int64_t>(child));
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__AST_DOT_VISUALISER_H
