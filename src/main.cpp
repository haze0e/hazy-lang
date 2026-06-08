#include "Parser.hpp"
#include "scanner.hpp"
#include "tokens.hpp"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

std::string read_file_contents(const std::string &filename);

int main(int argc, char *argv[]) {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;
  std::cerr << "Logs from your program will appear here!" << std::endl;

  if (argc < 3) {
    std::cerr << "Usage: ./your_program tokenize <filename>" << std::endl;
    return 1;
  }

  const std::string command = argv[1];
  std::string file_contents = read_file_contents(argv[2]);
  Scanner scanner(file_contents);
  scanner.scan_tokens();
  std::vector<token> my_tokens = scanner.get_tokens();
  Parser parser(my_tokens);
  std::unique_ptr<Expr> expr = parser.parse();
  if (command == "tokenize") {

    print_tokens(my_tokens);
    if (scanner.has_error()) {
      return 65;
    }
  }

  else if (command == "parse") {

    if (scanner.has_error()) {
      return 65; // Stop if scanner failed
    }

    if (parser.has_error()) {
      return 65;
    }

    if (expr) {
      std::cout << expr->toString() << std::endl;
    }
  } else if (command == "evaluate") {
    std::any result = expr->evaluate();
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
