#include <iostream>
#include "ast_dot_visualiser.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"
#include "variant_ostream.h"

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
    std::cout << fmt::format("Failed to interpret program_post. Error: {}", result.error()) << std::endl;
  } else {
    std::cout << "Memory dump: " << std::endl;
    for (const auto& entry : result->memory.data()) {
      std::cout << fmt::format("  {} = {}", entry.first, entry.second) << std::endl;
    }

    std::cout << std::endl;

    for (const auto& [name, symbol_table] : result->symbol_tables) {
      std::cout << symbol_table << std::endl;
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
   a : REAL;

PROCEDURE P1;
VAR
   b : REAL;
   k : INTEGER;

   PROCEDURE P2(c, d: INTEGER; e, f, g: REAL);
   VAR
      c, z : INTEGER;
   BEGIN {P2}
      z := 777.5;
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
