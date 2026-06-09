#pragma once
#include "Expr.hpp"
#include "evaluate.hpp"
#include <iostream>
#include <memory>

class Stmt {
public:
  virtual ~Stmt() = default;
  virtual void execute(Evaluator &evaluator) const = 0;
  virtual std::string toString() const = 0;
};

class PrintStmt : public Stmt {
public:
  const std::unique_ptr<Expr> expression;

  PrintStmt(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}

  void execute(Evaluator &evaluator) const override {
    std::any result = expression->accept(evaluator);

    if (!result.has_value()) {
      std::cout << "nil" << std::endl;
    } else if (result.type() == typeid(double)) {
      std::cout << std::any_cast<double>(result) << std::endl;
    } else if (result.type() == typeid(bool)) {
      std::cout << (std::any_cast<bool>(result) ? "true" : "false")
                << std::endl;
    } else if (result.type() == typeid(std::string)) {
      std::cout << std::any_cast<std::string>(result) << std::endl;
    } else {
      std::cout << "unknown type" << std::endl;
    }
  }

  std::string toString() const override {
    return "(print " + expression->toString() + ")";
  }
};

class ExpressionStmt : public Stmt {
public:
  const std::unique_ptr<Expr> expression;

  ExpressionStmt(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}

  void execute(Evaluator &evaluator) const override {
    expression->accept(evaluator);
  }

  std::string toString() const override { return expression->toString(); }
};
