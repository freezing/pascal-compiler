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
#include <fmt/format.h>
#include "result.h"

namespace freezing::interpreter {

enum class SymbolType {
  TYPE_SPECIFICATION,
  VARIABLE,
};

using Symbol = std::variant<SymbolType>;

class SymbolTable {
public:
  SymbolTable() = default;
  SymbolTable(const std::string& name);
  SymbolTable(std::string&& name);

  Result<Void> define(const std::string& symbol_name, Symbol&& symbol);

  std::optional<Symbol> find(const std::string& symbol_name) const;

  const std::map<std::string, Symbol>& data() const;

  const std::string& name() const;

private:
  std::string name_;
  std::map<std::string, Symbol> symbols_;

  void initialize();
};

std::ostream& operator<<(std::ostream& os, const SymbolType& symbol_type);
std::ostream& operator<<(std::ostream& os, const Symbol& symbol);
std::ostream& operator<<(std::ostream& os, const SymbolTable& symbol_table);

}

template<>
struct fmt::formatter<freezing::interpreter::SymbolTable>
    : freezing::interpreter::StringStreamFormatter<freezing::interpreter::SymbolTable> {
};

#endif //PASCAL_COMPILER_TUTORIAL__SYMBOL_TABLE_H
