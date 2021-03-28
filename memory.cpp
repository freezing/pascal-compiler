//
// Created by nikola on 3/28/2021.
//

#include "memory.h"

namespace freezing::interpreter {

int Memory::read(const std::string& address) const {
  auto it = memory_.find(address);
  assert(it != memory_.end());
  return it->second;
}

void Memory::set(const std::string& address, int value) {
  memory_[address] = value;
}

}