//
// Created by nikola on 3/28/2021.
//

#include "memory.h"

namespace freezing::interpreter {

DataType Memory::read(const std::string& address) const {
  auto it = memory_.find(address);
  assert(it != memory_.end());
  return it->second;
}

void Memory::set(const std::string& address, DataType value) {
  memory_[address] = value;
}
std::optional<DataType> Memory::try_read(const std::string& address) const {
  auto it = memory_.find(address);
  if (it == memory_.end()) {
    return {};
  }
  return it->second;
}

const std::map<std::string, DataType>& Memory::data() const {
  return memory_;
}

}