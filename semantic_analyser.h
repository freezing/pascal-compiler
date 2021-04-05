//
// Created by nikola on 3/20/2021.
//

#ifndef PASCAL_COMPILER_TUTORIAL__SEMANTIC_ANALYSER_H
#define PASCAL_COMPILER_TUTORIAL__SEMANTIC_ANALYSER_H

#include <iostream>
#include "ast_visitor.h"
#include "symbol_table.h"

namespace freezing::interpreter {

struct SemanticAnalysisError {
  std::string message;

  friend std::ostream& operator<<(std::ostream& os, const SemanticAnalysisError& error) {
    return os << error.message;
  }
};

class SemanticAnalyser {
public:
  Result<std::map<std::string, SymbolTable>, std::vector<SemanticAnalysisError>> analyse(const std::string& text,
                                                                                         const Program& program) const;
};

}

#endif //PASCAL_COMPILER_TUTORIAL__SEMANTIC_ANALYSER_H
