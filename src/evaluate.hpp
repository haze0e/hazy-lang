#pragma once
#include "Expr.hpp"
#include <stdexcept>

class RuntimeError : public std::runtime_error {
public:
  const token op;
  RuntimeError(const token &op, const std::string &message)
      : std::runtime_error(message), op(op) {}
};

class Evaluator : public ExprVisitor {
public:
  std::any visitBinaryExpr(const Binary &expr) override {
    std::any left_val = expr.left->accept(*this);
    std::any right_val = expr.right->accept(*this);

    auto isEqual = [](const std::any &a, const std::any &b) {
      if (!a.has_value() && !b.has_value())
        return true;
      if (!a.has_value() || !b.has_value())
        return false;
      if (a.type() != b.type())
        return false;
      if (a.type() == typeid(double))
        return std::any_cast<double>(a) == std::any_cast<double>(b);
      if (a.type() == typeid(std::string))
        return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
      if (a.type() == typeid(bool))
        return std::any_cast<bool>(a) == std::any_cast<bool>(b);
      return false;
    };

    switch (expr.op.token_type) {
    case type::MINUS:
      if (left_val.type() == typeid(double) &&
          right_val.type() == typeid(double)) {
        return std::any_cast<double>(left_val) -
               std::any_cast<double>(right_val);
      }
      throw RuntimeError(expr.op, "Operands must be numbers.");
    case type::SLASH:
      if (left_val.type() == typeid(double) &&
          right_val.type() == typeid(double)) {
        return std::any_cast<double>(left_val) /
               std::any_cast<double>(right_val);
      }
      throw RuntimeError(expr.op, "Operands must be numbers.");
    case type::STAR:
      if (left_val.type() == typeid(double) &&
          right_val.type() == typeid(double)) {
        return std::any_cast<double>(left_val) *
               std::any_cast<double>(right_val);
      }
      throw RuntimeError(expr.op, "Operands must be numbers.");
    case type::PLUS: {
      if (left_val.type() == typeid(double) &&
          right_val.type() == typeid(double)) {
        return std::any_cast<double>(left_val) +
               std::any_cast<double>(right_val);
      }
      if (left_val.type() == typeid(std::string) &&
          right_val.type() == typeid(std::string)) {
        return std::any_cast<std::string>(left_val) +
               std::any_cast<std::string>(right_val);
      }
      throw RuntimeError(expr.op,
                         "Operands must be two numbers or two strings.");
    }
    case type::GREATER:
      return std::any_cast<double>(left_val) > std::any_cast<double>(right_val);
    case type::GREATER_EQUAL:
      return std::any_cast<double>(left_val) >=
             std::any_cast<double>(right_val);
    case type::LESS:
      return std::any_cast<double>(left_val) < std::any_cast<double>(right_val);
    case type::LESS_EQUAL:
      return std::any_cast<double>(left_val) <=
             std::any_cast<double>(right_val);
    case type::BANG_EQUAL:
      return !isEqual(left_val, right_val);
    case type::EQUAL_EQUAL:
      return isEqual(left_val, right_val);
    default:
      return {};
    }
  }

  std::any visitGroupingExpr(const Grouping &expr) override {
    return expr.expression->accept(*this);
  }

  std::any visitLiteralExpr(const Literal &expr) override { return expr.value; }

  std::any visitUnaryExpr(const Unary &expr) override {
    std::any right_val = expr.right->accept(*this);

    switch (expr.op.token_type) {
    case type::MINUS:
      if (right_val.type() != typeid(double)) {
        throw RuntimeError(expr.op, "Operand must be a number.");
      }
      return -std::any_cast<double>(right_val);
    case type::BANG: {
      if (!right_val.has_value())
        return true;
      if (right_val.type() == typeid(bool))
        return !std::any_cast<bool>(right_val);
      return false;
    }
    default:
      return {};
    }
  }
};
