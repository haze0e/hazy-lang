#include <any>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

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
  UNKNOWN

};

std::string to_string(type s) {
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

std::string read_file_contents(const std::string &filename);

int main(int argc, char *argv[]) {
  // Disable output buffering
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  // You can use print statements as follows for debugging, they'll be visible
  // when running tests.
  std::cerr << "Logs from your program will appear here!" << std::endl;

  if (argc < 3) {
    std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
    return 1;
  }

  const std::string command = argv[1];

  if (command == "tokenize") {
    long long line = 1;
    class token {
    private:
    public:
      type token_type;
      std::string lexeme;
      std::any literal;
      long long line;
      token(type token_type, std::string lexeme, long long line = 1,
            std::any literal = std::any{})
          : token_type(token_type), lexeme(lexeme), literal(literal),
            line(line) {}
    };

    std::vector<token> tokens;
    bool have_error = 0;

    std::string file_contents = read_file_contents(argv[2]);

    for (int i = 0; i < file_contents.length(); i++) {
      switch (file_contents[i]) {
      case '(':
        tokens.push_back(token(type::LEFT_PAREN, "("));
        break;
      case ')':
        tokens.push_back(token(type::RIGHT_PAREN, ")"));
        break;
      case '}':
        tokens.push_back(token(type::RIGHT_BRACE, "}"));
        break;
      case '{':
        tokens.push_back(token(type::LEFT_BRACE, "{"));
        break;
      case '*':
        tokens.push_back(token(type::STAR, "*"));
        break;
      case '.':
        tokens.push_back(token(type::DOT, "."));
        break;
      case ',':
        tokens.push_back(token(type::COMMA, ","));
        break;
      case '-':
        tokens.push_back(token(type::MINUS, "-"));
        break;
      case '+':
        tokens.push_back(token(type::PLUS, "+"));
        break;
      case ';':
        tokens.push_back(token(type::SEMICOLON, ";"));
        break;
      case '\n':
        line++;
        break;
      default:
        have_error = 1;
        tokens.push_back(
            token(type::UNKNOWN, std::string(1, file_contents[i]), line));
        break;
      }
      // some operation
    }
    tokens.push_back(token(type::END_OF_FILE, ""));
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
    if (have_error) {
      return 65;
    }

  } else {
    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
  }

  return 0;
}

std::string read_file_contents(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    std::cerr << "Error reading file: " << filename << std::endl;
    std::exit(1);
  }

  std::stringstream buffer;
  buffer << file.rdbuf();
  file.close();

  return buffer.str();
}
