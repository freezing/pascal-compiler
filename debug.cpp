//
// Created by nikola on 4/5/2021.
//

#include "debug.h"

namespace freezing::interpreter {

std::string debug_output(const std::string& text, CharLocation location) {
  std::stringstream ss;

  int line_number = 0;
  int col_number = 0;
  bool add_line_number = true;
  for (int i = 0; i < text.size(); i++) {
    if (abs(location.line_number - line_number) <= 2) {
      if (add_line_number) {
        ss << std::setw(6) << line_number << "|";
        add_line_number = false;
      }
      ss << text[i];
    }

    col_number++;
    if (text[i] == '\n') {
      add_line_number = true;
      line_number++;
      col_number = 0;

      // This will ignore the errors on the last line, but that's okay.
      if (line_number == location.line_number + 1) {
        for (int j = 0; j < location.column_number - 1 + 7; j++) {
          ss << "-";
        }
        ss << "^" << std::endl;
      }
    }
  }
  return ss.str();
}

}