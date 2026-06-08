#include "tokens.hpp"
#include <vector>

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

public:
  Scanner(std::string source_code) : source(std::move(source_code)) {}

  void scan_tokens();

  const std::vector<token> &get_tokens() const { return tokens; }

  bool has_error() const { return have_error; }
};