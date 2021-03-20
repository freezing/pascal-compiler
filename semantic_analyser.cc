//
// Created by nikola on 3/20/2021.
//

#include "semantic_analyser.h"

namespace freezing::interpreter {

Result<SymbolTable, std::vector<std::string>> SemanticAnalyser::analyse(const Program& program) const {
  SymbolTable symbol_table;
  std::vector<std::string> errors;

  AstVisitorCallbacks callbacks{};

  callbacks.var_decl_pre = [&symbol_table, &errors](const VarDecl& var_decl) {
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

  callbacks.variable = [&symbol_table, &errors](const Variable& variable) {
    auto symbol = symbol_table.find(variable.name);
    if (!symbol) {
      errors.push_back(fmt::format("Undefined symbol: {}", variable.name));
    }
  };

  AstVisitorFn{callbacks}.visit(program);

  if (errors.empty()) {
    return symbol_table;
  } else {
    return make_error(std::move(errors));
  }
}

}