//
// Created by nikola on 3/10/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__CONTAINER_ALGO_H
#define PASCAL_COMPILER_TUTORIAL__CONTAINER_ALGO_H

#include <algorithm>

namespace freezing::interpreter {

template<typename T, typename IterT>
bool contains(IterT begin, IterT end, const T& value) {
  auto it = std::find(begin, end, value);
  return it != end;
}

template<typename KeyT, typename IterT>
bool contains_key(IterT begin, IterT end, const KeyT& key) {
  auto it = std::find_if(begin, end, [&key](const auto& entry) {
    return entry.first == key;
  });
  return it != end;
}

template<typename MapT, typename KeyT>
bool contains_key(const MapT& map, const KeyT& key) {
  return contains_key(std::begin(map), std::end(map), key);
}

}

#endif //PASCAL_COMPILER_TUTORIAL__CONTAINER_ALGO_H
