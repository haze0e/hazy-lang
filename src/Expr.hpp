#pragma once
#include "tokens.hpp"
#include <any>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

// grammar rules
/*
expression     → equality ;
equality       → comparison ( ( "!=" | "==" ) comparison )* ;
comparison     → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term           → factor ( ( "-" | "+" ) factor )* ;
factor         → unary ( ( "/" | "*" ) unary )* ;
unary          → ( "!" | "-" ) unary
               | primary ;
primary        → NUMBER | STRING | "true" | "false" | "nil"
               | "(" expression ")" ;
*/

class Binary;
class Grouping;
class Literal;
class Unary;

class ExprVisitor {
public:
  virtual ~ExprVisitor() = default;
  virtual std::any visitBinaryExpr(const Binary &expr) = 0;
  virtual std::any visitGroupingExpr(const Grouping &expr) = 0;
  virtual std::any visitLiteralExpr(const Literal &expr) = 0;
  virtual std::any visitUnaryExpr(const Unary &expr) = 0;
};

class Expr {
public:
  virtual ~Expr() = default;
  virtual std::any accept(ExprVisitor &visitor) const = 0;

  virtual std::string toString() const = 0;
  virtual std::any evaluate() const = 0;
};

class Binary : public Expr {
public:
  const std::unique_ptr<Expr> left;
  const token op;
  const std::unique_ptr<Expr> right;

  Binary(std::unique_ptr<Expr> left, token op, std::unique_ptr<Expr> right)
      : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

  std::any accept(ExprVisitor &visitor) const override {
    return visitor.visitBinaryExpr(*this);
  }

  std::string toString() const override {
    return "(" + op.lexeme + " " + left->toString() + " " + right->toString() +
           ")";
  }

  std::any evaluate() const override {
    std::any left_val = left->evaluate();
    std::any right_val = right->evaluate();

    auto isEqual = [](const std::any& a, const std::any& b) {
      if (!a.has_value() && !b.has_value()) return true;
      if (!a.has_value() || !b.has_value()) return false;
      if (a.type() != b.type()) return false;
      if (a.type() == typeid(double)) return std::any_cast<double>(a) == std::any_cast<double>(b);
      if (a.type() == typeid(std::string)) return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
      if (a.type() == typeid(bool)) return std::any_cast<bool>(a) == std::any_cast<bool>(b);
      return false;
    };

    switch (op.token_type) {
      case type::MINUS:
        return std::any_cast<double>(left_val) - std::any_cast<double>(right_val);
      case type::SLASH:
        return std::any_cast<double>(left_val) / std::any_cast<double>(right_val);
      case type::STAR:
        return std::any_cast<double>(left_val) * std::any_cast<double>(right_val);
      case type::PLUS: {
        if (left_val.type() == typeid(double) && right_val.type() == typeid(double)) {
          return std::any_cast<double>(left_val) + std::any_cast<double>(right_val);
        }
        if (left_val.type() == typeid(std::string) && right_val.type() == typeid(std::string)) {
          return std::any_cast<std::string>(left_val) + std::any_cast<std::string>(right_val);
        }
        throw std::runtime_error("Operands must be two numbers or two strings.");
      }
      case type::GREATER:
        return std::any_cast<double>(left_val) > std::any_cast<double>(right_val);
      case type::GREATER_EQUAL:
        return std::any_cast<double>(left_val) >= std::any_cast<double>(right_val);
      case type::LESS:
        return std::any_cast<double>(left_val) < std::any_cast<double>(right_val);
      case type::LESS_EQUAL:
        return std::any_cast<double>(left_val) <= std::any_cast<double>(right_val);
      case type::BANG_EQUAL:
        return !isEqual(left_val, right_val);
      case type::EQUAL_EQUAL:
        return isEqual(left_val, right_val);
      default:
        return {};
    }
  }
};

class Grouping : public Expr {
public:
  const std::unique_ptr<Expr> expression;

  explicit Grouping(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}

  std::any accept(ExprVisitor &visitor) const override {
    return visitor.visitGroupingExpr(*this);
  }

  std::string toString() const override {
    return "(group " + expression->toString() + ")";
  }

  std::any evaluate() const override {
    return expression->evaluate();
  }
};

class Literal : public Expr {
public:
  const std::any value;

  explicit Literal(std::any value) : value(std::move(value)) {}

  std::any accept(ExprVisitor &visitor) const override {
    return visitor.visitLiteralExpr(*this);
  }

  std::string toString() const override {
    if (!value.has_value())
      return "nil";

    if (value.type() == typeid(double)) {
      std::string str = std::to_string(std::any_cast<double>(value));
      str.erase(str.find_last_not_of('0') + 1, std::string::npos);
      if (str.back() == '.')
        str += "0";
      return str;
    }
    if (value.type() == typeid(std::string)) {
      return std::any_cast<std::string>(value);
    }
    if (value.type() == typeid(bool)) {
      return std::any_cast<bool>(value) ? "true" : "false";
    }
    return "unknown";
  }

  std::any evaluate() const override {
    return value;
  }
};

class Unary : public Expr {
public:
  const token op;
  const std::unique_ptr<Expr> right;

  Unary(token op, std::unique_ptr<Expr> right)
      : op(std::move(op)), right(std::move(right)) {}

  std::any accept(ExprVisitor &visitor) const override {
    return visitor.visitUnaryExpr(*this);
  }

  std::string toString() const override {
    return "(" + op.lexeme + " " + right->toString() + ")";
  }

  std::any evaluate() const override {
    std::any right_val = right->evaluate();

    switch (op.token_type) {
      case type::MINUS:
        return -std::any_cast<double>(right_val);
      case type::BANG: {
        if (!right_val.has_value()) return true;
        if (right_val.type() == typeid(bool)) return !std::any_cast<bool>(right_val);
        return false;
      }
      default:
        return {};
    }
  }
};
