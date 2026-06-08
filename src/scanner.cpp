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
    default:
      have_error = true;
      make_token(type::UNKNOWN, std::string(1, source[i]));
      break;
    }
  }
  make_token(type::END_OF_FILE, "");
}