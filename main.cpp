#include <iostream>
#include "ast_dot_visualiser.h"
#include "lexer.h"
#include "parser.h"

using namespace freezing::interpreter;

int main() {
  std::string pascal_program = R"(
    BEGIN
      BEGIN
        number := 2;
        a := number;
        b := 10 * a + 10 * number / 4;
        c := a - - b
      END;
      x := 11;
    END.
)";
  Parser parser{std::move(pascal_program)};
  auto ast = parser.parse_program();
  if (!ast) {
    std::cout << ast.error() << std::endl;
    return -1;
  }
  AstDotVisualiser visualiser{&ast.value()};
  std::cout << visualiser.generate() << std::endl;
  return 0;
}
