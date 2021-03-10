#include <iostream>
#include "ast_dot_visualiser.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

using namespace freezing::interpreter;

int visualise_ast(std::string&& pascal_program) {
  Parser parser{std::move(pascal_program)};
  auto ast = parser.parse_program();
  if (!ast) {
    std::cout << ast.error() << std::endl;
    return -1;
  }
  std::string dot = AstDotVisualiser{}.generate(*ast);
  std::cout << dot << std::endl;
  return 0;
}

int run_interpreter(std::string&& pascal_program) {
  auto result = Interpreter{}.run(std::move(pascal_program));
  if (!result) {
    std::cout << fmt::format("Failed to interpret program. Error: {}", result.error()) << std::endl;
  } else {
    std::cout << "Global scope: " << std::endl;
    for (const auto& entry : result->global_scope) {
      std::cout << fmt::format("  {} = {}", entry.first, entry.second) << std::endl;
    }
  }
  return 0;
}

int main() {
  std::string pascal_program = R"(
PROGRAM Part10AST;
VAR
   a, b : INTEGER;
   y    : REAL;

BEGIN {Part10AST}
   {a := 2;}
   b := 10 * a + 10 * a DIV 4;
   y := 20 / 7 + 3.14;
END.  {Part10AST}
)";
//  return visualise_ast(std::move(pascal_program));
  return run_interpreter(std::move(pascal_program));
}
