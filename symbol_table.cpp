//
// Created by nikola on 3/20/2021.
//

#include <fmt/format.h>
#include "token.h"
#include "symbol_table.h"

namespace freezing::interpreter {

struct SymbolPrinterFn {
  std::ostream& os;

  std::ostream& operator()(const SymbolType& symbol_type) {
    return os << symbol_type;
  }
};

std::ostream& operator<<(std::ostream& os, const SymbolType& symbol_type) {
  switch (symbol_type) {
  case SymbolType::VARIABLE:
    return os << "VARIABLE";
  case SymbolType::TYPE_SPECIFICATION:
    return os << "TYPE_SPECIFICATION";
  }
  return os << "UNKNOWN";
}

std::ostream& operator<<(std::ostream& os, const Symbol& symbol) {
  return std::visit(SymbolPrinterFn{os}, symbol);
}

SymbolTable::SymbolTable() {
  symbols[fmt::format("{}", TokenType::INTEGER)] = SymbolType::TYPE_SPECIFICATION;
  symbols[fmt::format("{}", TokenType::REAL)] = SymbolType::TYPE_SPECIFICATION;
}

Result<Void> SymbolTable::define(const std::string& symbol_name, Symbol&& symbol) {
  auto pair = symbols.insert({symbol_name, std::move(symbol)});
  if (!pair.second) {
    return make_error(fmt::format("Symbol '{}' is already defined.", symbol_name));
  }
  return {};
}

std::optional<Symbol> SymbolTable::find(const std::string& symbol_name) const {
  auto it = symbols.find(symbol_name);
  if (it == symbols.end()) {
    return {};
  }
  return it->second;
}

const std::map<std::string, Symbol>& SymbolTable::data() {
  return symbols;
}

}