#pragma once
#include "tokens.hpp"
#include <stdexcept>
#include <string>

class RuntimeError : public std::runtime_error {
public:
  const token op;
  RuntimeError(const token &op, const std::string &message)
      : std::runtime_error(message), op(op) {}
};
