//
// Created by nikola on 3/20/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H
#define PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H

namespace freezing::interpreter {

enum class SymbolType {
  TYPE_SPECIFICATION,
  VARIABLE,
};

using Symbol = std::variant<SymbolType>;

inline std::ostream& operator<<(std::ostream& os, const SymbolType& symbol_type) {
  switch (symbol_type) {
  case SymbolType::VARIABLE:
    return os << "VARIABLE";
  case SymbolType::TYPE_SPECIFICATION:
    return os << "TYPE_SPECIFICATION";
  }
  return os << "UNKNOWN";
}

struct SymbolPrinterFn {
  std::ostream& os;

  std::ostream& operator()(const SymbolType& symbol_type) {
    return os << symbol_type;
  }
};

inline std::ostream& operator<<(std::ostream& os, const Symbol& symbol) {
  return std::visit(SymbolPrinterFn{os}, symbol);
}

class SymbolTable {
public:
  SymbolTable() {
    symbols[fmt::format("{}", TokenType::INTEGER)] = SymbolType::TYPE_SPECIFICATION;
    symbols[fmt::format("{}", TokenType::REAL)] = SymbolType::TYPE_SPECIFICATION;
  }

  Result<Void> define(const std::string& symbol_name, Symbol&& symbol) {
    auto pair = symbols.insert({symbol_name, std::move(symbol)});
    if (!pair.second) {
      return make_error(fmt::format("Symbol '{}' is already defined.", symbol_name));
    }
    return {};
  }

  std::optional<Symbol> find(const std::string& symbol_name) const {
    auto it = symbols.find(symbol_name);
    if (it == symbols.end()) {
      return {};
    }
    return it->second;
  }

  const std::map<std::string, Symbol>& data() {
    return symbols;
  }

private:
  std::map<std::string, Symbol> symbols;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H
