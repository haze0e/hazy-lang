#pragma once
#include "RuntimeError.hpp"
#include "tokens.hpp"
#include <any>
#include <string>
#include <unordered_map>

class Environment {
private:
  std::unordered_map<std::string, std::any> values;

public:
  void define(const std::string &name, std::any value) { values[name] = value; }

  std::any get(const token &name) {
    if (values.find(name.lexeme) != values.end()) {
      return values[name.lexeme];
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const token &name, std::any value) {
    if (values.find(name.lexeme) != values.end()) {
      values[name.lexeme] = value;
      return;
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }
};
