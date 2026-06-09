# Hazy Lang

An elegantly designed, dynamically typed scripting language interpreter written entirely from scratch in modern C++17.

## 🚀 Features

Hazy Lang is currently in active development, featuring a fully functional tree-walk interpreter architecture:

- **Robust Lexical Scanner**: Converts source code into an easily parsable stream of tokens.
- **Recursive Descent Parser**: A powerful, hand-written parser that builds an Abstract Syntax Tree (AST).
- **Tree-Walk Evaluator**: Executes the AST directly with full support for runtime type checking.
- **Dynamic Typing**: Native support for `double`, `bool`, `string`, and `nil` types.
- **State & Variables**: Supports global variable declarations (`var x = 5;`) and environment memory management.
- **Statements & Expressions**: Full support for complex mathematical operations, logical comparisons, and side-effects (e.g. `print` statements).

## 🛠 Architecture

Hazy Lang strictly separates the parsing and evaluation phases to ensure maximum extensibility:

1. **Scanning**: `scanner.cpp` reads raw text and groups characters into meaningful `token` objects.
2. **Parsing**: `Parser.hpp` processes tokens left-to-right using recursive descent to enforce grammar rules and construct an object-oriented AST.
3. **Evaluating**: `evaluate.hpp` relies on a clean `execute()` / `accept()` visitor-like architecture to traverse the AST, throwing `RuntimeError` objects when invalid operations occur.

## ⚙️ Building Locally

Hazy Lang uses CMake for its build system.

```bash
# Clone the repository
git clone https://github.com/haze0e/hazy-lang.git
cd hazy-lang

# Build using CMake
cmake -B build
cmake --build build

# Run the interpreter!
./build/interpreter evaluate test.lox
```

## 📝 Example Usage

Create a file named `test.lox`:
```js
var greeting = "Hello from Hazy Lang!";
print greeting;

var a = 12;
var b = 24;
print a + b; // 36
```

Run it:
```bash
./build/interpreter run test.lox
```

---
*Built with ❤️ in C++17.*
