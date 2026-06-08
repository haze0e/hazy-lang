#include "scanner.hpp"
#include "tokens.hpp"

void Scanner::scan_tokens() {
  for (size_t i = 0; i < source.length(); i++) {
    switch (source[i]) {
    case '(':
      make_token(type::LEFT_PAREN, "(");
      break;
    case ')':
      make_token(type::RIGHT_PAREN, ")");
      break;
    case '}':
      make_token(type::RIGHT_BRACE, "}");
      break;
    case '{':
      make_token(type::LEFT_BRACE, "{");
      break;
    case '*':
      make_token(type::STAR, "*");
      break;
    case '.':
      make_token(type::DOT, ".");
      break;
    case ',':
      make_token(type::COMMA, ",");
      break;
    case '-':
      make_token(type::MINUS, "-");
      break;
    case '+':
      make_token(type::PLUS, "+");
      break;
    case ';':
      make_token(type::SEMICOLON, ";");
      break;
    case '\n':
      line++;
      break;
    case ' ':
    case '\r':
    case '\t':
      break;

    case '!':
      if (i + 1 < source.length() && source[i + 1] == '=') {
        make_token(type::BANG_EQUAL, source.substr(i, 2));
        i++;
      } else {
        make_token(type::BANG, source.substr(i, 1));
      }
      break;
    case '=':
      if (i + 1 < source.length() && source[i + 1] == '=') {
        make_token(type::EQUAL_EQUAL, source.substr(i, 2));
        i++;
      } else {
        make_token(type::EQUAL, source.substr(i, 1));
      }
      break;
    case '<':
      if (i + 1 < source.length() && source[i + 1] == '=') {
        make_token(type::LESS_EQUAL, source.substr(i, 2));
        i++;
      } else {
        make_token(type::LESS, source.substr(i, 1));
      }
      break;
    case '>':
      if (i + 1 < source.length() && source[i + 1] == '=') {
        make_token(type::GREATER_EQUAL, source.substr(i, 2));
        i++;
      } else {
        make_token(type::GREATER, source.substr(i, 1));
      }
      break;

    case '/':
      if (i + 1 < source.length() && source[i + 1] == '/') {
        while (source[i] != '\n' && i < (source.size())) {
          i++;
        }
        if (source[i] == '\n') {
          line++;
        }

      } else {
        make_token(type::SLASH, source.substr(i, 1));
      }
      break;

    case '"': {
      int start = i;
      while (i + 1 < source.length() && source[i + 1] != '"') {
        if (source[i + 1] == '\n') {
          line++;
        }
        i++;
      }

      if (i + 1 >= source.length()) {
        have_error = true;
        error("Unterminated string.");
      } else {
        i++;
        std::string lexeme = source.substr(start, i - start + 1);
        std::string literal = lexeme.substr(1, lexeme.length() - 2);
        make_token(type::STRING, lexeme, literal);
      }
      break;
    }

    default:
      if (is_digit(source[i])) {
        int start = i;
        while (i + 1 < source.length() && is_digit(source[i + 1])) {
          i++;
        }

        if (i + 1 < source.length() && source[i + 1] == '.') {
          if (i + 2 < source.length() && is_digit(source[i + 2])) {
            i++;
            while (i + 1 < source.length() && is_digit(source[i + 1])) {
              i++;
            }
          }
        }

        std::string lexeme = source.substr(start, i - start + 1);
        make_token(type::NUMBER, lexeme, std::stod(lexeme));
      }

      else if (is_alpha(source[i])) {
        int start = i;
        while (i + 1 < source.length() && is_alphanumeric(source[i + 1])) {
          i++;
        }
        std::string lexeme = source.substr(start, i - start + 1);

        auto it = keywords.find(lexeme);
        if (it != keywords.end()) {
          make_token(it->second, lexeme);
        } else {
          make_token(type::IDENTIFIER, lexeme);
        }
      } else {
        have_error = true;
        error("Unexpected character: " + std::string(1, source[i]));
      }
      break;
    }
  }
  make_token(type::END_OF_FILE, "");
}

void Scanner::error(std::string message) {
  std::cerr << "[line " << line << "] Error: " << message << std::endl;
}