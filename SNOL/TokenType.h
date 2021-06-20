#pragma once

#include <string>

enum TokenType {
  // Single-character tokens.
  LEFT_PAREN, RIGHT_PAREN,
  DOT, MINUS, PLUS, SLASH, STAR, MODULO,

  EQUAL,

  // Literals.
  IDENTIFIER, INT, FLOAT,

  // Keywords.
  BEG, PRINT,

  END_OF_FILE,
};

std::string toString(TokenType type) {
  static const std::string strings[] = {
    "LEFT_PAREN", "RIGHT_PAREN",
    "DOT", "MINUS", "PLUS", "SLASH", "STAR", "MODULO",
    "EQUAL",
    "IDENTIFIER", "INT", "FLOAT",
    "BEG", "PRINT",
    "END_OF_FILE"
  };

  return strings[static_cast<int>(type)];
}
