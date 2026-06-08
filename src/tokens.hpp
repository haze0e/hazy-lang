#include <any>
#include <iostream>
#include <string>
#include <vector>
#pragma once

enum class type {
  END_OF_FILE,
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  STAR,
  DOT,
  COMMA,
  MINUS,
  PLUS,
  SEMICOLON,
  UNKNOWN,

  // double char tokens

  BANG_EQUAL,
  EQUAL,
  EQUAL_EQUAL,
  BANG,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL,

};

std::string inline to_string(type s) {
  switch (s) {
  case type::END_OF_FILE:
    return "EOF";
  case type::LEFT_PAREN:
    return "LEFT_PAREN";
  case type::RIGHT_PAREN:
    return "RIGHT_PAREN";
  case type::LEFT_BRACE:
    return "LEFT_BRACE";
  case type::RIGHT_BRACE:
    return "RIGHT_BRACE";
  case type::STAR:
    return "STAR";
  case type::DOT:
    return "DOT";
  case type::COMMA:
    return "COMMA";
  case type::MINUS:
    return "MINUS";
  case type::PLUS:
    return "PLUS";
  case type::SEMICOLON:
    return "SEMICOLON";
  default:
    return "UNKNOWN";
  }
}

class token {
public:
  type token_type;
  std::string lexeme;
  std::any literal;
  long long line;
  token(type token_type, std::string lexeme, long long line,
        std::any literal = std::any{})
      : token_type(token_type), lexeme(lexeme), literal(literal), line(line) {}
};

void inline print_tokens(const std::vector<token> &tokens) {
  for (int i = 0; i < tokens.size(); i++) {

    if (!(tokens[i].token_type == type::UNKNOWN)) {
      std::cout << to_string(tokens[i].token_type) + " " << tokens[i].lexeme
                << " ";
      if (tokens[i].literal.has_value()) {
        if (auto str_val = std::any_cast<std::string>(&tokens[i].literal)) {
          std::cout << *str_val << std::endl;
        } else if (auto double_val =
                       std::any_cast<double>(&tokens[i].literal)) {
          std::cout << *double_val << std::endl;
        } else {
          std::cout << "unknown" << std::endl;
        }
      } else {
        std::cout << "null" << std::endl;
      }
      // some operation
    } else {
      std::cerr << "[line " << tokens[i].line
                << "] Error: Unexpected character: " << tokens[i].lexeme
                << std::endl;
    }
  }
}