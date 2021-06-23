#pragma once

#include <iostream>
#include <string_view>
#include "RuntimeError.h"
#include "Token.h"

void report(std::string_view where,
                   std::string_view message, bool& hadError) {
  std::cerr <<
      "SNOL> Error! " << message <<
      "\n";
  hadError = true;
}

void error(const Token& token, std::string_view message, bool& hadError) {
  if (token.type == END_OF_FILE) {
    report(" at end", message, hadError);
  } else {
    report(" at '" + token.lexeme + "'", message, hadError);
  }
}

void error(std::string_view message, bool& hadError) {
  report("", message, hadError);
}

void runtimeError(const RuntimeError& error, bool& hadRuntimeError) {
  std::cerr <<
      "SNOL> " << error.what()
      << "\n";
  hadRuntimeError = true;
}
