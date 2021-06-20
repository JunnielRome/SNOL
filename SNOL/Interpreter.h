#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility>        // std::move
#include "Environment.h"
#include "Error.h"
#include "Expr.h"
#include "RuntimeError.h"
#include "Stmt.h"

class Interpreter: public ExprVisitor,
                   public StmtVisitor {
  std::shared_ptr<Environment> environment{new Environment};

public:
  void interpret(const std::vector<
      std::shared_ptr<Stmt>>& statements) {
    try {
      for (const std::shared_ptr<Stmt>& statement : statements) {
        execute(statement);
      }
    } catch (RuntimeError error) {
      runtimeError(error);
    }
  }

private:
  std::any evaluate(std::shared_ptr<Expr> expr) {
    return expr->accept(*this);
  }

  void execute(std::shared_ptr<Stmt> stmt) {
    stmt->accept(*this);
  }

public:
  std::any visitExpressionStmt(
      std::shared_ptr<Expression> stmt) override {
    evaluate(stmt->expression);
    return {};
  }

  std::any visitPrintStmt(std::shared_ptr<Print> stmt) override {
    std::any value = evaluate(stmt->expression);
    std::cout << "SNOL> " << stringify(value) << "\n";
    return {};
  }

  std::any visitBegStmt(std::shared_ptr<Beg> stmt) override {
    std::any value = nullptr;
    bool isNumber = false;
    bool isInt = false;
    std::string line;
    char *temp;

    std::cout << "SNOL> Please enter value for [" << stmt->name.lexeme << "]:\n";
    while(!isNumber) {
      std::cout << "Input: ";
      std::getline(std::cin, line);

      strtod(line.c_str(), &temp);
      if(!strlen(temp))
        isNumber = true;
      if(line.find(".") == -1)
        isInt = true;
      if(line[0] == '.')
        isNumber = false;
      if(!isNumber)
        std::cout << "\nSNOL> Must be an integer or float! Please enter again.\n";
    }

    if (isInt)
      value = std::stoi(line);
    else
      value = std::stod(line);

    environment->assign(stmt->name, std::move(value));
    return {};
  }

  std::any visitAssignExpr(std::shared_ptr<Assign> expr) override {
    std::any value = evaluate(expr->value);
    environment->assign(expr->name, value);
    return value;
  }

  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
    std::any left = evaluate(expr->left);
    std::any right = evaluate(expr->right);


    if (left.type() == typeid(int) &&
        right.type() == typeid(int)){
      switch (expr->op.type) {
        case MINUS:;
          return std::any_cast<int>(left) -
                std::any_cast<int>(right);
        case PLUS:
            return std::any_cast<int>(left) +
                  std::any_cast<int>(right);
        case SLASH:
          return std::any_cast<int>(left) /
                std::any_cast<int>(right);
        case STAR:
          return std::any_cast<int>(left) *
                std::any_cast<int>(right);
        case MODULO:
          return std::any_cast<int>(left) %
                std::any_cast<int>(right);
      }
    }
    else if (left.type() == typeid(double) &&
        right.type() == typeid(double)){
      switch (expr->op.type) {
        case MINUS:;
          return std::any_cast<double>(left) -
                std::any_cast<double>(right);
        case PLUS:
            return std::any_cast<double>(left) +
                  std::any_cast<double>(right);
        case SLASH:
          return std::any_cast<double>(left) /
                std::any_cast<double>(right);
        case STAR:
          return std::any_cast<double>(left) *
                std::any_cast<double>(right);
        case MODULO:
          throw RuntimeError{expr->op,
              "Operands must be of the same type in an arithmetic operation!"};

      }
    }
    else{
      throw RuntimeError{expr->op,
          "Operands must be of the same type in an arithmetic operation!"};
    }

    // Unreachable.
    return {};
  }

  std::any visitGroupingExpr(
      std::shared_ptr<Grouping> expr) override {
    return evaluate(expr->expression);
  }

  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
    return expr->value;
  }

  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override {
    std::any right = evaluate(expr->right);
    switch (expr->op.type) {
      case MINUS:
        checkNumberOperand(expr->op, right);
        if (right.type() == typeid(int)) return -std::any_cast<int>(right);
        if (right.type() == typeid(double)) return -std::any_cast<double>(right);
    }

    // Unreachable.
    return {};
  }

  std::any visitVariableExpr(
      std::shared_ptr<Variable> expr) override {
    return environment->get(expr->name);
  }

private:
  void checkNumberOperand(const Token& op,
                          const std::any& operand) {
    if (operand.type() == typeid(int)) return;
    if (operand.type() == typeid(double)) return;
    throw RuntimeError{op, "Operand must be a number."};
  }

  void checkNumberOperands(const Token& op,
                           const std::any& left,
                           const std::any& right) {
    if (left.type() == right.type()) {
      return;
    }

    throw RuntimeError{op, "Operands must be numbers."};
  }

  bool isTruthy(const std::any& object) {
    if (object.type() == typeid(nullptr)) return false;
    if (object.type() == typeid(bool)) {
      return std::any_cast<bool>(object);
    }
    return true;
  }

  std::string stringify(const std::any& object) {
    if (object.type() == typeid(nullptr)) return "null";

    if (object.type() == typeid(int)) {
      std::string text = std::to_string(
          std::any_cast<int>(object));
      return text;
    }

    if (object.type() == typeid(double)) {
      std::string text = std::to_string(
          std::any_cast<double>(object));
      text.erase(text.find_last_not_of('0') + 1, std::string::npos); 
      if(text.back() == '.')
        text.push_back('0');
      return text;
    }

    if (object.type() == typeid(std::string)) {
      return std::any_cast<std::string>(object);
    }

    return "Error in stringify: object type not recognized.";
  }
};
