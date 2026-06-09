#pragma once
#include "Expr.hpp"
// #include "statement.hpp"
#include "statement.hpp"
#include "tokens.hpp"
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>
#include <vector>

// grammar rules
/*

program        → declaration* EOF ;

declaration    → varDecl
               | statement ;

statement      → exprStmt
               | printStmt ;



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

class Parser {
private:
  const std::vector<token> tokens;
  int current = 0;
  bool had_error = false;

  std::unique_ptr<Stmt> declaration() {
    if (match(type::VAR)) {
      return VariableStatement();
    }
    return statement();
  }

  std::unique_ptr<Stmt> VariableStatement() {
    token name = consume(type::IDENTIFIER, "Expect variable name.");
    std::unique_ptr<Expr> initializer = nullptr;
    if (match(type::EQUAL)) {
      initializer = expression();
    }
    consume(type::SEMICOLON, "Expect ';' after variable declaration.");
    return std::make_unique<VarStmt>(name, std::move(initializer));
  }

  std::unique_ptr<Stmt> statement() {
    if (match(type::PRINT)) {
      return printStatement();
    }
    return ExpressionStatement();
  }

  std::unique_ptr<Stmt> printStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(type::SEMICOLON, "Expect ';' after value.");
    return std::make_unique<PrintStmt>(std::move(value));
  }

  std::unique_ptr<Stmt> ExpressionStatement() {
    std::unique_ptr<Expr> value = expression();
    consume(type::SEMICOLON, "Expect ';' after expression.");
    return std::make_unique<ExpressionStmt>(std::move(value));
  }

  std::unique_ptr<Expr> expression() { return equality(); }

  std::unique_ptr<Expr> equality() {
    std::unique_ptr<Expr> expr = comparison();

    while (match(type::BANG_EQUAL, type::EQUAL_EQUAL)) {
      token op = previous();
      std::unique_ptr<Expr> right = comparison();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }

    return expr;
  }

  std::unique_ptr<Expr> comparison() {
    std::unique_ptr<Expr> expr = term();
    while (match(type::GREATER, type::GREATER_EQUAL, type::LESS,
                 type::LESS_EQUAL)) {
      token op = previous();
      std::unique_ptr<Expr> right = term();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expr> term() {
    std::unique_ptr<Expr> expr = factor();
    while (match(type::MINUS, type::PLUS)) {
      token op = previous();
      std::unique_ptr<Expr> right = factor();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expr> factor() {
    std::unique_ptr<Expr> expr = unary();
    while (match(type::SLASH, type::STAR)) {
      token op = previous();
      std::unique_ptr<Expr> right = unary();
      expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::unique_ptr<Expr> unary() {
    if (match(type::MINUS, type::BANG)) {
      token op = previous();
      std::unique_ptr<Expr> right = unary();
      return std::make_unique<Unary>(op, std::move(right));
    } else {
      return primary();
    }
  }

  std::unique_ptr<Expr> primary() {
    if (match(type::FALSE))
      return std::make_unique<Literal>(false);
    if (match(type::TRUE))
      return std::make_unique<Literal>(true);
    if (match(type::NIL))
      return std::make_unique<Literal>(std::any());

    if (match(type::NUMBER, type::STRING)) {
      return std::make_unique<Literal>(previous().literal);
    }

    if (match(type::LEFT_PAREN)) {
      std::unique_ptr<Expr> expr = expression();
      consume(type::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_unique<Grouping>(std::move(expr));
    }

    if (match(type::IDENTIFIER)) {
      return std::make_unique<VariableExpr>(previous());
    }

    throw error(peek(), "Expect expression.");
  }

  token consume(type t, const std::string &message) {
    if (check(t))
      return advance();

    throw error(peek(), message);
  }

  template <typename... T> bool match(T... types) {
    for (type t : {types...}) {
      if (check(t)) {
        advance();
        return true;
      }
    }
    return false;
  }

  bool check(type t) {
    if (isAtEnd())
      return false;
    return peek().token_type == t;
  }

  token advance() {
    if (!isAtEnd())
      current++;
    return previous();
  }

  bool isAtEnd() { return peek().token_type == type::END_OF_FILE; }

  token peek() { return tokens[current]; }

  token previous() { return tokens[current - 1]; }

  std::runtime_error error(const token &tok, const std::string &message) {
    had_error = true;
    if (tok.token_type == type::END_OF_FILE) {
      std::cerr << "[line " << tok.line << "] Error at end: " << message
                << std::endl;
    } else {
      std::cerr << "[line " << tok.line << "] Error at '" << tok.lexeme
                << "': " << message << std::endl;
    }
    return std::runtime_error(message);
  }

  void synchronize() {
    advance();

    while (!isAtEnd()) {
      if (previous().token_type == type::SEMICOLON)
        return;

      switch (peek().token_type) {
      case type::CLASS:
      case type::FUN:
      case type::VAR:
      case type::FOR:
      case type::IF:
      case type::WHILE:
      case type::PRINT:
      case type::RETURN:
        return;
      default:
        break;
      }

      advance();
    }
  }

public:
  Parser(const std::vector<token> &tokens) : tokens(tokens) {}

  std::vector<std::unique_ptr<Stmt>> parse() {
    std::vector<std::unique_ptr<Stmt>> statements;
    while (!isAtEnd()) {
      try {
        statements.push_back(declaration());
      } catch (const std::runtime_error &) {
        // If there's an error, synchronize so we can try to parse the NEXT
        // statement
        synchronize();
      }
    }
    return statements;
  }

  std::unique_ptr<Expr> parse_expression() {
    try {
      return expression();
    } catch (const std::runtime_error &) {
      return nullptr;
    }
  }

  bool has_error() const { return had_error; }
};
