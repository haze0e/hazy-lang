#include "tokens.hpp"
#include <vector>
#pragma once

class Scanner {
private:
  long long line = 1;
  bool have_error = false;
  std::vector<token> tokens;
  std::string source;

  void make_token(type token_type, std::string lexeme,
                  std::any literal = std::any{}) {
    tokens.push_back(token(token_type, lexeme, line, literal));
  }

  bool is_digit(char c) {
    return c >= '0' && c <= '9';
  }

  bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
  }

  bool is_alphanumeric(char c) {
    return is_alpha(c) || is_digit(c);
  }

public:
  Scanner(std::string source_code) : source(std::move(source_code)) {}

  void scan_tokens();
  void error(std::string message);

  const std::vector<token> &get_tokens() const { return tokens; }

  bool has_error() const { return have_error; }
};