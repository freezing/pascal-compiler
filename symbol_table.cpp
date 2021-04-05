//
// Created by nikola on 3/20/2021.
//

#include <iostream>
#include <iomanip>
#include <fmt/format.h>
#include "token.h"
#include "symbol_table.h"
#include "variant_match.h"

namespace freezing::interpreter {

SymbolTable::SymbolTable(const std::string& name) : name_{name} {
  initialize();
}

SymbolTable::SymbolTable(std::string&& name) : name_{std::move(name)} {
  initialize();
}

void SymbolTable::initialize() {
  symbols_[fmt::format("{}", TokenType::INTEGER)] = TypeSpecificationSymbol{};
  symbols_[fmt::format("{}", TokenType::REAL)] = TypeSpecificationSymbol{};
}

Result<Void> SymbolTable::define(const std::string& symbol_name, Symbol&& symbol) {
  auto pair = symbols_.insert({symbol_name, std::move(symbol)});
  if (!pair.second) {
    return make_error(fmt::format("Symbol '{}' is already defined.", symbol_name));
  }
  return {};
}

std::optional<Symbol> SymbolTable::find(const std::string& symbol_name) const {
  auto it = symbols_.find(symbol_name);
  if (it == symbols_.end()) {
    return {};
  }
  return it->second;
}

const std::map<std::string, Symbol>& SymbolTable::data() const {
  return symbols_;
}

const std::string& SymbolTable::name() const {
  return name_;
}
std::optional<ProcedureHeaderSymbol> SymbolTable::find_procedure_header(const std::string& procedure_name) const {
  auto symbol = find(procedure_name);
  if (!symbol) {
    return {};
  }
  return variant_try_get<ProcedureHeaderSymbol>(*symbol);
}

std::ostream& operator<<(std::ostream& os, const VariableSymbol& symbol) {
  return os << "VariableSymbol";
}

std::ostream& operator<<(std::ostream& os, const TypeSpecificationSymbol& symbol) {
  return os << "TypeSpecificationSymbol";
}

std::ostream& operator<<(std::ostream& os, const ProcedureHeaderSymbol& symbol) {
  return os << fmt::format("ProcedureHeaderSymbol(name={}, num_args={})", symbol.name, symbol.parameters.size());
}

std::ostream& operator<<(std::ostream& os, const Symbol& symbol) {
  return std::visit([&os](auto&& arg) -> std::ostream& { return os << arg; }, symbol);
}

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbol_table) {
  os << "SymbolTable(" << symbol_table.name() << ")" << std::endl;
  for (const auto &[key, value] : symbol_table.data()) {
    os << std::setw(10) << key << " -> " << value << std::endl;
  }
  return os;
}

}