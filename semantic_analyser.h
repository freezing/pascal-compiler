//
// Created by nikola on 3/20/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__SEMANTIC_ANALYSER_H
#define PASCAL_COMPILER_TUTORIAL__SEMANTIC_ANALYSER_H

#include "ast_visitor.h"
#include "symbol_table.h"

namespace freezing::interpreter {

class SemanticAnalyser {
public:
  Result<std::map<std::string, SymbolTable>, std::vector<std::string>> analyse(const Program& program) const;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__SEMANTIC_ANALYSER_H
