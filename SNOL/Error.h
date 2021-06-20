#pragma once

#include <iostream>
#include <string_view>
#include "RuntimeError.h"
#include "Token.h"

inline bool hadError = false;
inline bool hadRuntimeError = false;

void report(std::string_view where,
                   std::string_view message) {
  std::cerr <<
      "SNOL> Error! " << message <<
      "\n";
  hadError = true;
}

void error(const Token& token, std::string_view message) {
  if (token.type == END_OF_FILE) {
    report(" at end", message);
  } else {
    report(" at '" + token.lexeme + "'", message);
  }
}

void error(std::string_view message) {
  report("", message);
}

void runtimeError(const RuntimeError& error) {
  std::cerr <<
      "SNOL> " << error.what()
      << "\n";
  hadRuntimeError = true;
}
