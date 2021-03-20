//
// Created by nikola on 3/20/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H
#define PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H

#include <variant>
#include <ostream>
#include <optional>
#include <map>
#include <string>
#include "result.h"

namespace freezing::interpreter {

enum class SymbolType {
  TYPE_SPECIFICATION,
  VARIABLE,
};

using Symbol = std::variant<SymbolType>;

std::ostream& operator<<(std::ostream& os, const SymbolType& symbol_type);
std::ostream& operator<<(std::ostream& os, const Symbol& symbol);

class SymbolTable {
public:
  SymbolTable();

  Result<Void> define(const std::string& symbol_name, Symbol&& symbol);

  std::optional<Symbol> find(const std::string& symbol_name) const;

  const std::map<std::string, Symbol>& data();

private:
  std::map<std::string, Symbol> symbols;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H
