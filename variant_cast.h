//
// Created by nikola on 4/3/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__VARIANT_CAST_H
#define PASCAL_COMPILER_TUTORIAL__VARIANT_CAST_H

namespace freezing::interpreter {

template<typename V1, typename V2>
struct variant_cast_extractor_fn;

template<typename From, typename...To>
struct variant_cast_operator {
  std::variant<To...> operator()(const From& from) const {
    return from;
  }
};

template<typename...To, typename...From>
struct variant_cast_extractor_fn<std::variant<To...>, std::variant<From...>> : variant_cast_operator<From, To...>... {
  using variant_cast_operator<From, To...>::operator()...;
};

template<typename...Args>
struct variant_cast_proxy {
  std::variant<Args...> v;

  template<typename...To>
  operator std::variant<To...>() const {
    return std::visit(variant_cast_extractor_fn<std::variant<To...>, std::variant<Args...>>{}, v);
  }
};

template<typename...Args>
variant_cast_proxy<Args...> variant_cast(const std::variant<Args...>& value) {
  return {value};
}

}

#endif //PASCAL_COMPILER_TUTORIAL__VARIANT_CAST_H
