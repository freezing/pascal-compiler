//
// Created by nikola on 3/20/2021.
//

#include <iostream>
#include <iomanip>
#include <fmt/format.h>
#include "token.h"
#include "symbol_table.h"

namespace freezing::interpreter {

SymbolTable::SymbolTable(const std::string& name) : name_{name} {
  initialize();
}

SymbolTable::SymbolTable(std::string&& name) : name_{std::move(name)} {
  initialize();
}

void SymbolTable::initialize() {
  symbols_[fmt::format("{}", TokenType::INTEGER)] = SymbolType::TYPE_SPECIFICATION;
  symbols_[fmt::format("{}", TokenType::REAL)] = SymbolType::TYPE_SPECIFICATION;
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

std::ostream& operator<<(std::ostream& os, const SymbolTable& symbol_table) {
  os << "SymbolTable(" << symbol_table.name() << ")" << std::endl;
  for (const auto &[key, value] : symbol_table.data()) {
    os << std::setw(10) << key << " -> " << value << std::endl;
  }
  return os;
}

}