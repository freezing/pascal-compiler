//
// Created by nikola on 4/3/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__VARIANT_MATCH_H
#define PASCAL_COMPILER_TUTORIAL__VARIANT_MATCH_H

template<class... Ts> struct overload : Ts... { using Ts::operator()...; };
template<class... Ts> overload(Ts...) -> overload<Ts...>;

#endif //PASCAL_COMPILER_TUTORIAL__VARIANT_MATCH_H
