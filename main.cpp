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

    std::cout << std::endl << "Symbol table: " << std::endl;
    for (const auto& entry : result->symbol_table.data()) {
      std::cout << fmt::format("  {} = {}", entry.first, entry.second) << std::endl;
    }

    std::cout << std::endl << "Errors: " << std::endl;
    for (const auto& error : result->errors) {
      std::cout << error << std::endl;
    }
  }
  return 0;
}

int main() {
  std::string pascal_program = R"(
PROGRAM Part12;
VAR
   a : INTEGER;

PROCEDURE P1;
VAR
   a : REAL;
   k : INTEGER;

   PROCEDURE P2;
   VAR
      a, z : INTEGER;
   BEGIN {P2}
      z := 777;
   END;  {P2}

BEGIN {P1}

END;  {P1}

BEGIN {Part12}
   a := 10;
END.  {Part12}
)";
//  return visualise_ast(std::move(pascal_program));
  return run_interpreter(std::move(pascal_program));
}
