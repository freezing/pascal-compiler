//
// Created by nikola on 3/28/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__MEMORY_H
#define PASCAL_COMPILER_TUTORIAL__MEMORY_H

#include <string>
#include <map>
#include <cassert>
#include <variant>
#include <optional>

namespace freezing::interpreter {

using DataType = std::variant<int, double>;

inline std::ostream& operator<<(std::ostream& os, const DataType& data) {
  struct PrintFn {
    std::ostream& os;

    std::ostream& operator()(int value) {
      return os << value;
    }

    std::ostream& operator()(double value) {
      return os << value;
    }
  };
  return std::visit(PrintFn{os}, data);
}

class Memory {
public:
  DataType read(const std::string& address) const;

  std::optional<DataType> try_read(const std::string& address) const;

  void set(const std::string& address, DataType value);

  const std::map<std::string, DataType>& data() const;

private:
  std::map<std::string, DataType> memory_;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__MEMORY_H
