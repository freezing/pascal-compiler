//
// Created by nikola on 3/10/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__ID_GENERATOR_H
#define PASCAL_COMPILER_TUTORIAL__ID_GENERATOR_H

template<typename T>
class IdGenerator {
public:
  IdGenerator() : next_id{} {}
  
  T next() {
    return next_id++;
  }

private:
  T next_id;
};

#endif //PASCAL_COMPILER_TUTORIAL__ID_GENERATOR_H
