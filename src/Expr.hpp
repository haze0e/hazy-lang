#pragma once
#include "tokens.hpp"
#include <any>
#include <memory>
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
class VariableExpr;
class AssignExpr;

class ExprVisitor {
public:
  virtual ~ExprVisitor() = default;
  virtual std::any visitBinaryExpr(const Binary &expr) = 0;
  virtual std::any visitGroupingExpr(const Grouping &expr) = 0;
  virtual std::any visitLiteralExpr(const Literal &expr) = 0;
  virtual std::any visitUnaryExpr(const Unary &expr) = 0;
  virtual std::any visitVariableExpr(const VariableExpr &expr) = 0;
  virtual std::any visitAssignExpr(const AssignExpr &expr) = 0;
};

class Expr {
public:
  virtual ~Expr() = default;
  virtual std::any accept(ExprVisitor &visitor) const = 0;

  virtual std::string toString() const = 0;
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
};

class VariableExpr : public Expr {
public:
  const token name;

  VariableExpr(token name) : name(std::move(name)) {}

  std::any accept(ExprVisitor &visitor) const override {
    return visitor.visitVariableExpr(*this);
  }

  std::string toString() const override { return name.lexeme; }
};

class AssignExpr : public Expr {
public:
  const token name;
  const std::unique_ptr<Expr> value;

  AssignExpr(token name, std::unique_ptr<Expr> value)
      : name(std::move(name)), value(std::move(value)) {}

  std::any accept(ExprVisitor &visitor) const override {
    return visitor.visitAssignExpr(*this);
  }

  std::string toString() const override {
    return "(assign " + name.lexeme + " " + value->toString() + ")";
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
};
