#include "Parser.hpp"
#include "evaluate.hpp"
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
  if (command == "tokenize") {
    print_tokens(my_tokens);
    if (scanner.has_error()) {
      return 65;
    }
    return 0;
  }

  if (scanner.has_error()) {
    return 65;
  }

  Parser parser(my_tokens);
  std::vector<std::unique_ptr<Stmt>> statements = parser.parse();
  if (parser.has_error()) {
    return 65;
  }
  if (command == "parse") {
    for (const auto &stmt : statements) {
      std::cout << stmt->toString() << std::endl;
    }
  } else if (command == "evaluate") {
    if (parser.has_error()) {
      return 65;
    }
    Evaluator evaluator;
    try {
      for (const auto &stmt : statements) {
        stmt->execute(evaluator);
      }
    } catch (const RuntimeError &e) {
      std::cerr << e.what() << "\n[line " << e.op.line << "]" << std::endl;
      return 70;
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
