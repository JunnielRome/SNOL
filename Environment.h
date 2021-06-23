#pragma once

#include <any>
#include <functional> // less
#include <map>
#include <memory>
#include <string>
#include <utility>    // std::move
#include "Error.h"
#include "Token.h"

class Environment: public std::enable_shared_from_this<Environment> {
  std::map<std::string, std::any> values;

public:
  std::any get(const Token& name) {
    auto elem = values.find(name.lexeme);
    if (elem != values.end()) {
      return elem->second;
    }

    throw RuntimeError(name,
        "Error! [" + name.lexeme + "] is not defined!");
  }

  void assign(const Token& name, std::any value) {
    auto elem = values.find(name.lexeme);
    if (elem != values.end()) {
      elem->second = std::move(value);
    }
    // if variable is not defined then we define it
    else{
      define(name.lexeme, std::move(value));
    }
    return;
  }

  void define(const std::string& name, std::any value) {
    values[name] = std::move(value);

  }
};
