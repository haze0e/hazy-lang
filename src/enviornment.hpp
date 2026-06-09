#pragma once
#include "RuntimeError.hpp"
#include "tokens.hpp"
#include <any>
#include <cstddef>
#include <memory>
#include <string>
#include <unordered_map>

class Environment {
private:
  std::unordered_map<std::string, std::any> values;

public:
  std::shared_ptr<Environment> parent;

  Environment() : parent(nullptr) {};
  Environment(std::shared_ptr<Environment> parent) : parent(parent) {};
  void define(const std::string &name, std::any value) { values[name] = value; }

  std::any get(const token &name) {
    if (values.find(name.lexeme) != values.end()) {
      return values[name.lexeme];
    }

    if (parent != nullptr) {
      return parent->get(name);
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  void assign(const token &name, std::any value) {
    if (values.find(name.lexeme) != values.end()) {
      values[name.lexeme] = value;
      return;
    }

    if (parent != nullptr) {
      parent->assign(name, value);
    }

    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }
};
