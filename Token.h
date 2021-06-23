#pragma once

#include <any>
#include <string>
#include <utility>      // std::move
#include "TokenType.h"

class Token {
public:
  const TokenType type;
  const std::string lexeme;
  const std::any literal;

  Token(TokenType type, std::string lexeme, std::any literal)
    : type{type}, lexeme{std::move(lexeme)},
      literal{std::move(literal)}
  {}

  std::string toString() const {
    std::string literal_text;

    switch (type) {
      case (IDENTIFIER):
        literal_text = lexeme;
        break;
      case (INT):
        literal_text = std::to_string(std::any_cast<int>(literal));
        break;
      case (FLOAT):
        literal_text = std::to_string(std::any_cast<double>(literal));
        break;
      default:
        literal_text = "null";
    }

    return ::toString(type) + " " + lexeme + " " + literal_text;
  }
};
