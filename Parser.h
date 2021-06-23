#pragma once

#include <cassert>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>      // std::move
#include <vector>
#include "Error.h"
#include "Expr.h"
#include "Stmt.h"
#include "Token.h"
#include "TokenType.h"

class Parser {
  struct ParseError: public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  const std::vector<Token>& tokens;
  int current = 0;
  bool hadError = false;

public:
  Parser(const std::vector<Token>& tokens)
    : tokens{tokens}
  {}

  std::vector<std::shared_ptr<Stmt>> parse(bool& fromError) {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
      // statements.push_back(statement());
      statements.push_back(declaration());
    }

    fromError = hadError;

    return statements;
  }

private:
  std::shared_ptr<Expr> expression() {
    return assignment();
  }

  std::shared_ptr<Stmt> declaration() {
    try {
      if (match(BEG)) return begDeclaration();

      return statement();
    } catch (ParseError error) {
      synchronize();
      return nullptr;
    }
  }

  std::shared_ptr<Stmt> statement() {
    if (match(PRINT)) return printStatement();

    return expressionStatement();
  }

  std::shared_ptr<Stmt> printStatement() {
    std::shared_ptr<Expr> value = expression();
    if (match(IDENTIFIER)){
      consume(IDENTIFIER, "Unknown Command! Does not match any valid command of the language.");
    }
    return std::make_shared<Print>(value);
  }

  std::shared_ptr<Stmt> begDeclaration() {
    Token name = consume(IDENTIFIER, "Expect variable name.");

    std::shared_ptr<Expr> initializer = nullptr;
    if (match(IDENTIFIER)){
      consume(IDENTIFIER, "Unknown Command! Does not match any valid command of the language.");
    }

    return std::make_shared<Beg>(std::move(name), initializer);
  }

  std::shared_ptr<Stmt> expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    
    if (match(IDENTIFIER)){
      consume(IDENTIFIER, "Unknown Command! Does not match any valid command of the language.");
    }

    return std::make_shared<Expression>(expr);
  }

  std::shared_ptr<Expr> assignment() {
    std::shared_ptr<Expr> expr = equality();

    if (match(EQUAL)) {
      Token equals = previous();
      std::shared_ptr<Expr> value = assignment();

      if (Variable* e = dynamic_cast<Variable*>(expr.get())) {
        Token name = e->name;
        return std::make_shared<Assign>(std::move(name), value);
      }

      error(std::move(equals), "Invalid assignment target.");
    }

    return expr;
  }

  std::shared_ptr<Expr> equality() {
    std::shared_ptr<Expr> expr = comparison();

    return expr;
  }

  std::shared_ptr<Expr> comparison() {
    std::shared_ptr<Expr> expr = term();

    return expr;
  }

  std::shared_ptr<Expr> term() {
    std::shared_ptr<Expr> expr = factor();

    while (match(MINUS, PLUS)) {
      Token op = previous();
      std::shared_ptr<Expr> right = factor();
      expr = std::make_shared<Binary>(expr, std::move(op), right);
    }

    return expr;
  }

  std::shared_ptr<Expr> factor() {
    std::shared_ptr<Expr> expr = unary();

    while (match(SLASH, STAR, MODULO)) {
      Token op = previous();
      std::shared_ptr<Expr> right = unary();
      expr = std::make_shared<Binary>(expr, std::move(op), right);
    }

    return expr;
  }

  std::shared_ptr<Expr> unary() {
    if (match(MINUS)) {
      Token op = previous();
      std::shared_ptr<Expr> right = unary();
      return std::make_shared<Unary>(std::move(op), right);
    }

    return primary();
  }

  std::shared_ptr<Expr> primary() {
    if (match(INT)) {
      return std::make_shared<Literal>(previous().literal);
    }

    if (match(FLOAT)) {
      return std::make_shared<Literal>(previous().literal);
    }

    if (match(IDENTIFIER)) {
      return std::make_shared<Variable>(previous());
    }

    if (match(LEFT_PAREN)) {
      std::shared_ptr<Expr> expr = expression();
      consume(RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(expr);
    }

    throw error(peek(), "Unknown command! Does not match any valid command of the language.");
  }

  template <class... T>
  bool match(T... type) {
    assert((... && std::is_same_v<T, TokenType>));

    if ((... || check(type))) {
      advance();
      return true;
    }

    return false;
  }

  Token consume(TokenType type, std::string_view message) {
    if (check(type)) return advance();

    throw error(peek(), message);
  }

  bool check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
  }

  Token advance() {
    if (!isAtEnd()) ++current;
    return previous();
  }

  bool isAtEnd() {
    return peek().type == END_OF_FILE;
  }

  Token peek() {
    return tokens.at(current);
  }

  Token previous() {
    return tokens.at(current - 1);
  }

  ParseError error(const Token& token, std::string_view message) {
    ::error(token, message, hadError);
    return ParseError{""};
  }

  void synchronize() {
    advance();

    while (!isAtEnd()) {

      switch (peek().type) {
        case BEG:
        case PRINT:
          return;
      }

      advance();
    }
  }
};