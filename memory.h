//
// Created by nikola on 3/28/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__MEMORY_H
#define PASCAL_COMPILER_TUTORIAL__MEMORY_H

#include <string>
#include <map>
#include <cassert>
#include <optional>

namespace freezing::interpreter {

class Memory {
public:
  int read(const std::string& address) const;

  std::optional<int> try_read(const std::string& address) const;

  void set(const std::string& address, int value);

  const std::map<std::string, int>& data() const;

private:
  // TODO: should support double values too.
  std::map<std::string, int> memory_;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__MEMORY_H
