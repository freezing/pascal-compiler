//
// Created by nikola on 3/2/2021.
//

#include "ast.h"
#include "ast_visitor.h"

#ifndef PASCAL_COMPILER_TUTORIAL__AST_DOT_VISUALISER_H
#define PASCAL_COMPILER_TUTORIAL__AST_DOT_VISUALISER_H

namespace freezing::interpreter {

namespace detail {

std::string format_node(NodeId node_id, const std::string& label) {
  return fmt::format(
      R"(  node{} [label="{}"]
)",
      node_id, label);
}

std::string format_node_edge(NodeId parent_id, NodeId child_id) {
  return fmt::format(
      R"(  node{} -> node{}
)",
      parent_id,
      child_id);
}

}

class AstDotVisualiser {
public:
  explicit AstDotVisualiser() {}

  std::string generate(const Program& program) {
    std::string body{};

    AstVisitorCallbacks callbacks{};

    callbacks.program_post = [&body](const Program& program) {
      body += detail::format_node(program.id, fmt::format("Program({})", program.name));
      body += detail::format_node_edge(program.id, program.block.id);
    };
    callbacks.block = [&body](const Block& block) {
      body += detail::format_node(block.id, fmt::format("Block"));
      for (const auto& decl : block.variable_declarations) {
        body += detail::format_node_edge(block.id, decl.id);
      }
      for (const auto& proc_decl : block.procedure_declarations) {
        body += detail::format_node_edge(block.id, proc_decl.id);
      }
      body += detail::format_node_edge(block.id, block.compound_statement.id);
    };
    callbacks.var_decl_post = [&body](const VarDecl& var_decl) {
      body += detail::format_node(var_decl.id, fmt::format("VarDecl(type={})", var_decl.type_specification));
      for (const auto& variable : var_decl.variables) {
        body += detail::format_node_edge(var_decl.id, variable.id);
      }
    };
    callbacks.procedure_decl_post = [&body](const ProcedureDecl& procedure_decl) {
      body += detail::format_node(procedure_decl.id, fmt::format("ProcedureDecl({})", procedure_decl.name));
      for (const auto& param : procedure_decl.parameters) {
        body += detail::format_node_edge(procedure_decl.id, param.id);
      }
      body += detail::format_node_edge(procedure_decl.id, procedure_decl.block.id);
    };
    callbacks.param = [&body](const Param& param) {
      body += detail::format_node(param.id, fmt::format("Param({}: {})", param.identifier, param.type_specification));
    };
    callbacks.compound_statement = [&body](const CompoundStatement& compound_statement) {
      body += detail::format_node(compound_statement.id, "CompoundStatement");
      for (const auto& statement : compound_statement.statements) {
        body += detail::format_node_edge(compound_statement.id, std::visit(NodeIdExtractorFn{}, statement));
      }
    };
    callbacks.assignment_statement = [&body](const AssignmentStatement& assignment_statement) {
      body += detail::format_node(assignment_statement.id, ":=");
      body += detail::format_node_edge(assignment_statement.id, assignment_statement.variable.id);
      body += detail::format_node_edge(assignment_statement.id,
                                       std::visit(NodeIdExtractorFn{}, assignment_statement.expression));
    };
    callbacks.procedure_call_post = [&body](const ProcedureCall& procedure_call) {
      body += detail::format_node(procedure_call.id, fmt::format("{}()", procedure_call.name));
      for (const auto& param : procedure_call.parameters) {
        body += detail::format_node_edge(procedure_call.id, std::visit(NodeIdExtractorFn{}, param));
      }
    };
    callbacks.unary_op = [&body](const UnaryOp& unary_op) {
      body += detail::format_node(unary_op.id, fmt::format("Unary({})", unary_op.op_type));
      body += detail::format_node_edge(unary_op.id, std::visit(NodeIdExtractorFn{}, *unary_op.node));
    };
    callbacks.bin_op = [&body](const BinOp& bin_op) {
      body += detail::format_node(bin_op.id, fmt::format("Binary({})", bin_op.op_type));
      body += detail::format_node_edge(bin_op.id, std::visit(NodeIdExtractorFn{}, *bin_op.left));
      body += detail::format_node_edge(bin_op.id, std::visit(NodeIdExtractorFn{}, *bin_op.right));
    };
    callbacks.variable = [&body](const Variable& variable) {
      body += detail::format_node(variable.id, fmt::format("Variable({})", variable.name));
    };
    callbacks.num = [&body](const Num& num) {
      body += detail::format_node(num.id, fmt::format("Num({})", num.value));
    };
    callbacks.empty = [&body](const Empty& empty) {
      body += detail::format_node(empty.id, "Empty");
    };

    AstVisitorFn{callbacks}.visit(program);

    const std::string header =
        R"(digraph astgraph {
           node [shape=circle, fontsize=12, fontname="Courier", height=.1];
           ranksep=.3;
           edge [arrowsize=.5]
)";
    const std::string footer = "}";
    return header + body + footer;
  }
};

}

#endif //PASCAL_COMPILER_TUTORIAL__AST_DOT_VISUALISER_H
