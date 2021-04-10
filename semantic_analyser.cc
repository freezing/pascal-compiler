//
// Created by nikola on 3/20/2021.
//

#include "semantic_analyser.h"

namespace freezing::interpreter {

Result<std::map<std::string, SymbolTable>,
       std::vector<SemanticAnalysisError>> SemanticAnalyser::analyse(const std::string& text,
                                                                     const Program& program) const {
  // Current scope is first initialized in the program_post callback.
  // It is safe to assume it's always present while traversing AST.
  std::optional<std::string> current_scope;
  std::map<std::string, SymbolTable> scopes;
  std::map<std::string, std::string> parent_scopes;
  std::vector<SemanticAnalysisError> errors;

  AstVisitorCallbacks callbacks{};

  callbacks.program_pre = [&scopes, &current_scope](const Program& program) {
    scopes[program.name] = SymbolTable{program.name};
    current_scope = program.name;
  };

  // TODO: Implement type checking.

  callbacks.procedure_decl_pre =
      [&scopes, &current_scope, &parent_scopes, &errors](const ProcedureDecl& procedure_decl) {
        auto insert = scopes.emplace(procedure_decl.name, SymbolTable{procedure_decl.name});
        if (!insert.second) {
          errors.push_back(SemanticAnalysisError{
              fmt::format("Scope with name '{}' already exists.", procedure_decl.name)});
          return;
        }
        // Insert procedure in the current scope.
        auto define_proc = scopes[*current_scope]
            .define(procedure_decl.name,
                    ProcedureHeaderSymbol{procedure_decl.name, procedure_decl.parameters, procedure_decl.block});
        if (!define_proc) {
          errors.push_back(SemanticAnalysisError{define_proc.error()});
        }
        // Update parent and current scope.
        parent_scopes[procedure_decl.name] = *current_scope;
        current_scope = procedure_decl.name;

        auto& symbol_table = insert.first->second;
        for (const auto& param : procedure_decl.parameters) {
          symbol_table.define(param.identifier, VariableSymbol{});
        }
      };

  callbacks.procedure_decl_post = [&current_scope, &parent_scopes](const ProcedureDecl& procedure_decl) {
    current_scope = parent_scopes[*current_scope];
  };

  callbacks.procedure_call_post =
      [&scopes, &current_scope, &parent_scopes, &errors](const ProcedureCall& procedure_call) {
        int num_passed_args = procedure_call.parameters.size();

        // Lookup procedure symbol for each scope between this and root of the tree.
        // TODO: Extract into helper method.
        std::string tmp_scope = *current_scope;
        std::optional<ProcedureHeaderSymbol> procedure_symbol{};
        while (true) {
          procedure_symbol = scopes[tmp_scope].find_procedure_header(procedure_call.name);
          if (procedure_symbol) {
            break;
          }
          auto parent_it = parent_scopes.find(tmp_scope);
          if (parent_it == parent_scopes.end()) {
            break;
          }
          tmp_scope = parent_it->second;
        }

        // TODO: Ast nodes require metadata about the location in the text to provide better debug messages.
        if (!procedure_symbol) {
          errors.push_back(SemanticAnalysisError{fmt::format("Calling undefined procedure: {}", procedure_call.name)});
        } else if (procedure_symbol->parameters.size() != num_passed_args) {
          errors.push_back(SemanticAnalysisError{fmt::format("Procedure '{}' expects {} arguments, but got {}",
                                                             procedure_call.name,
                                                             procedure_symbol->parameters.size(),
                                                             num_passed_args)});
        }
      };

  callbacks.var_decl_pre = [&scopes, &current_scope, &errors](const VarDecl& var_decl) {
    assert(current_scope.has_value());
    auto scope_it = scopes.find(*current_scope);
    assert(scope_it != scopes.end());
    auto& symbol_table = scope_it->second;

    auto type_symbol = symbol_table.find(fmt::format("{}", var_decl.type_specification));
    if (!type_symbol) {
      errors.push_back(SemanticAnalysisError{fmt::format("Unknown type symbol: '{}'", var_decl.type_specification)});
    }
    std::vector<Identifier> already_defined_symbols;
    for (const auto& variable_symbol : var_decl.variables) {
      auto existing_symbol = symbol_table.find(variable_symbol.name);
      if (existing_symbol) {
        already_defined_symbols.push_back(variable_symbol.name);
      }
      symbol_table.define(variable_symbol.name, VariableSymbol{});
    }
    if (!already_defined_symbols.empty()) {
      std::string error = "[";
      std::string sep;
      for (const auto& s : already_defined_symbols) {
        error += sep + s;
        sep = ", ";
      }
      error += "]";
      errors.push_back(SemanticAnalysisError{fmt::format("Already defined symbols: {}", error)});
    }
  };

  callbacks.variable = [&scopes, &current_scope, &errors](const Variable& variable) {
    assert(current_scope.has_value());
    auto scope_it = scopes.find(*current_scope);
    assert(scope_it != scopes.end());
    auto& symbol_table = scope_it->second;

    auto symbol = symbol_table.find(variable.name);
    if (!symbol) {
      errors.push_back(SemanticAnalysisError{fmt::format("Undefined symbol: {}", variable.name)});
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