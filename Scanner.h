#pragma once

#include <map>
#include <string>
#include <string_view>
#include <utility>      // std::move
#include <vector>
#include "Error.h"
#include "Token.h"

class Scanner {
  static const std::map<std::string, TokenType> keywords;

  std::string_view source;
  std::vector<Token> tokens;
  int start = 0;
  int current = 0;
  bool hadError = false;

public:
  Scanner(std::string_view source)
    : source {source}
  {}

  std::vector<Token> scanTokens(bool& fromError) {
    while (!isAtEnd()) {
      // We are at the beginning of the next lexeme.
      start = current;
      scanToken();
    }

    tokens.emplace_back(END_OF_FILE, "", nullptr);

    fromError = hadError;
    return tokens;
  }

private:
  void scanToken() {
    char c = advance();
    switch (c) {
      case '(': addToken(LEFT_PAREN); break;
      case ')': addToken(RIGHT_PAREN); break;
      case '-': addToken(MINUS); break;
      case '+': addToken(PLUS); break;
      case '*': addToken(STAR); break;
      case '/': addToken(SLASH); break;
      case '%': addToken(MODULO); break;
      case '=': addToken(EQUAL); break;

      case ' ':
      case '\r':
      case '\t':
        // Ignore whitespace.
        break;

      default:
        // error("Unexpected character");

        if (isDigit(c)) {
          number();
        } else if (isAlpha(c)) {
          identifier();
        } else {
          error("Unexpected character.", hadError);
        }
        break;
      };
  }

  void identifier() {
    while (isAlphaNumeric(peek())) advance();

    // addToken(IDENTIFIER);

    std::string text =
        std::string{source.substr(start, current - start)};

    TokenType type;
    auto match = keywords.find(text);
    if (match == keywords.end()) {
      type = IDENTIFIER;
    } else {
      type = match->second;
    }

    addToken(type);
  }

  void number() {
    while (isDigit(peek())) advance();
    bool isInt = true;
    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext())) {
      isInt = false;
      // Consume the "."
      advance();

      while (isDigit(peek())) advance();
    }

    std::string temp = std::string{source.substr(start, current - start)};

    if (peek() == '.' && !isDigit(peekNext())){
      isInt = false;
      advance();
      temp.push_back('.');
      temp.push_back('0');
    }

    if (isInt)
      addToken(INT,
          std::stoi(temp));
    else
      addToken(FLOAT,
          std::stod(temp));
  }

  char peek() {
    if (isAtEnd()) return '\0';
    return source[current];
  }

  char peekNext() {
    if (current + 1 >= source.length()) return '\0';
    return source[current + 1];
  }

  bool isAlpha(char c) {
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z');
  }

  bool isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
  }

  bool isDigit(char c) {
    return c >= '0' && c <= '9';
  }

  bool isAtEnd() {
    return current >= source.length();
  }

  char advance() {
    return source[current++];
  }

  void addToken(TokenType type) {
    addToken(type, nullptr);
  }

  void addToken(TokenType type, std::any literal) {
    std::string text{source.substr(start, current - start)};
    tokens.emplace_back(type, std::move(text), std::move(literal));
  }
};

const std::map<std::string, TokenType> Scanner::keywords =
{
  {"BEG",    TokenType::BEG},
  {"PRINT",  TokenType::PRINT},
};
