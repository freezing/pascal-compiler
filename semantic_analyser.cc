//
// Created by nikola on 3/20/2021.
//

#include "semantic_analyser.h"

namespace freezing::interpreter {

Result<std::map<std::string, SymbolTable>, std::vector<std::string>> SemanticAnalyser::analyse(const Program& program) const {
  // Current scope is first initialized in the program_post callback.
  // It is safe to assume it's always present while traversing AST.
  std::optional<std::string> current_scope;
  std::map<std::string, SymbolTable> scopes;
  std::map<std::string, std::string> parent_scopes;
  std::vector<std::string> errors;

  AstVisitorCallbacks callbacks{};

  callbacks.program_pre = [&scopes, &current_scope](const Program& program) {
    scopes[program.name] = SymbolTable{program.name};
    current_scope = program.name;
  };

  callbacks.procedure_decl_pre = [&scopes, &current_scope, &parent_scopes, &errors](const ProcedureDecl& procedure_decl) {
    auto insert = scopes.emplace(procedure_decl.name, SymbolTable{procedure_decl.name});
    if (!insert.second) {
      errors.push_back(fmt::format("Scope with name '{}' already exists.", procedure_decl.name));
    }
    parent_scopes[procedure_decl.name] = *current_scope;
    current_scope = procedure_decl.name;
  };

  callbacks.procedure_decl_post = [&current_scope, &parent_scopes](const ProcedureDecl& procedure_decl) {
    current_scope = parent_scopes[*current_scope];
  };

  callbacks.var_decl_pre = [&scopes, &current_scope, &errors](const VarDecl& var_decl) {
    assert(current_scope.has_value());
    auto scope_it = scopes.find(*current_scope);
    assert(scope_it != scopes.end());
    auto& symbol_table = scope_it->second;

    auto type_symbol = symbol_table.find(fmt::format("{}", var_decl.type_specification));
    if (!type_symbol) {
      errors.push_back(fmt::format("Unknown type symbol: '{}'", var_decl.type_specification));
    }
    std::vector<Identifier> already_defined_symbols;
    for (const auto& variable_symbol : var_decl.variables) {
      auto existing_symbol = symbol_table.find(variable_symbol.name);
      if (existing_symbol) {
        already_defined_symbols.push_back(variable_symbol.name);
      }
      symbol_table.define(variable_symbol.name, SymbolType::VARIABLE);
    }
    if (!already_defined_symbols.empty()) {
      std::string error = "[";
      std::string sep;
      for (const auto& s : already_defined_symbols) {
        error += sep + s;
        sep = ", ";
      }
      error += "]";
      errors.push_back(fmt::format("Already defined symbols: {}", error));
    }
  };

  callbacks.variable = [&scopes, &current_scope, &errors](const Variable& variable) {
    assert(current_scope.has_value());
    auto scope_it = scopes.find(*current_scope);
    assert(scope_it != scopes.end());
    auto& symbol_table = scope_it->second;

    auto symbol = symbol_table.find(variable.name);
    if (!symbol) {
      errors.push_back(fmt::format("Undefined symbol: {}", variable.name));
    }
  };

  AstVisitorFn{callbacks}.visit(program);

  if (errors.empty()) {
    return scopes;
  } else {
    return make_error(std::move(errors));
  }
}

}