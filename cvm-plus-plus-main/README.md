# CVM++

A Turing-Complete, Stack-Based Virtual Machine and Bytecode Compiler written from scratch in C++17.

[![Language](https://img.shields.io/badge/Language-C%2B%2B17-00599C?style=flat-square&logo=cplusplus&logoColor=white)](https://isocpp.org)
[![Build](https://img.shields.io/badge/Build-CMake-064F8C?style=flat-square&logo=cmake&logoColor=white)](https://cmake.org)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg?style=flat-square)](LICENSE)

---

## Table of Contents

- [Overview](#overview)
- [Architecture Pipeline](#architecture-pipeline)
- [Language Features](#language-features)
- [Language Syntax and Examples](#language-syntax-and-examples)
- [Project Structure](#project-structure)
- [Prerequisites](#prerequisites)
- [Building from Source](#building-from-source)
- [Running CVM++](#running-cvm)
- [Debug Mode](#debug-mode)
- [Instruction Set Architecture](#instruction-set-architecture)
- [References](#references)

---

## Overview

CVM++ is a custom, dynamically-typed scripting language with a complete end-to-end compilation pipeline — built entirely in C++17 with zero external dependencies.

Most developers use high-level languages without deeply understanding how raw source text is translated into instructions a processor can execute. CVM++ demystifies this by implementing every layer of the stack from scratch: a Lexer, a Recursive Descent Parser, a Bytecode Compiler, and a Stack-Based Virtual Machine.

Unlike tree-walking interpreters, CVM++ separates the translation phase from the execution phase entirely. The VM execution loop operates on a flat, cache-friendly array of 1-byte opcodes rather than traversing a tree at runtime — prioritizing execution speed as a first principle.

---

## Architecture Pipeline

```
Source Code (.cvm)
        |
        v
+------------------+
|  Lexer           |   lexer.cpp / lexer.h
|  Tokenization    |   Raw text  -->  Token stream
+------------------+
        |
        v
+------------------+
|  Parser          |   parser.cpp / parser.h
|  Recursive       |   Token stream  -->  Abstract Syntax Tree (AST)
|  Descent         |   Enforces grammar and BODMAS precedence
+------------------+   Uses std::unique_ptr for node ownership
        |
        v
+------------------+
|  Compiler        |   compiler.cpp / compiler.h
|  Bytecode        |   AST (post-order traversal)  -->  Bytecode chunk
|  Generation      |   Backpatching for control flow jump offsets
+------------------+
        |
        v
+------------------+
|  Virtual Machine |   vm.cpp / vm.h
|  Stack-Based     |   Bytecode chunk  -->  Execution result
|  Execution       |   Instruction Pointer (ip) + std::vector data stack
+------------------+
```

### Stage Breakdown

**1. Lexical Analysis (`lexer.cpp`)**
Scans the raw source string character-by-character, classifying each sequence into a structured `Token` — keywords, operators, identifiers, numeric literals, and string literals. Strips whitespace and comments, producing a clean token stream for the parser.

**2. Recursive Descent Parser (`parser.cpp`)**
Consumes the token stream and enforces the language grammar. Operator precedence (BODMAS) is encoded directly into the recursive call hierarchy — lower-precedence rules call higher-precedence rules, ensuring correct parse trees without an explicit precedence table. Nodes are allocated as `std::unique_ptr<ASTNode>` to guarantee safe, leak-free ownership semantics.

**3. Bytecode Compiler (`compiler.cpp`)**
Performs a post-order (left-right-root) traversal of the AST. Each node emits one or more 1-byte `OpCode` instructions into a linear `Chunk` (`std::vector<uint8_t>`). Control flow (`if`/`else`, `while`) is implemented via **bytecode backpatching**: the compiler emits a placeholder jump instruction, records its offset, compiles the body, then patches the correct jump distance retrospectively.

**4. Stack-Based Virtual Machine (`vm.cpp`)**
The execution engine maintains an Instruction Pointer (`ip`) and a value stack (`std::vector`). The main loop is a fast-dispatch switch on the current opcode. Arithmetic operations pop operands, compute, and push results. Variable reads and writes interact with a `std::unordered_map` heap. Jump opcodes manipulate `ip` directly to implement control flow.

---

## Language Features

| Feature             | Details                                                               |
| ------------------- | --------------------------------------------------------------------- |
| Data Types          | Integer, Boolean                                                      |
| Arithmetic          | `+`, `-`, `*`, `/`                                                    |
| Comparisons         | `==`, `<`, `>`                                                        |
| Variables           | Dynamic, stored in a hash map (`std::unordered_map`)                  |
| Control Flow        | `if` / `else`, `while` loops                                          |
| I/O                 | `print` keyword, `input` keyword                                      |
| Turing Completeness | Arbitrary looping and conditional branching via bytecode jump offsets |

---

## Language Syntax and Examples

### Variables and Assignment

```
let x = 10;
let y = 25;
let sum = x + y;
print sum;
```

### Conditional Branching

```
let score = 85;

if (score > 50) {
    print 1;
} else {
    print 0;
}
```

### While Loop with Countdown

```
let countdown = 3;

while (countdown > 0) {
    print countdown;
    let countdown = countdown - 1;
}
```

### Combined: Conditional Inside Loop

```
let countdown = 3;

if (countdown > 0) {
    print 999;
} else {
    print 0;
}

while (countdown > 0) {
    print countdown;
    let countdown = countdown - 1;
}
```

Expected output:

```
999
3
2
1
```

---

## Project Structure

```
CVM++/
├── include/
│   ├── ast.h           # AST node definitions (ASTNode base + derived types)
│   ├── chunk.h         # Bytecode chunk: OpCode enum + instruction array
│   ├── compiler.h      # Compiler class: AST -> Chunk
│   ├── lexer.h         # Lexer class: source string -> Token stream
│   ├── parser.h        # Parser class: Token stream -> AST
│   ├── token.h         # Token struct and TokenType enum
│   └── vm.h            # VM class: Chunk execution engine
│
├── src/
│   ├── compiler.cpp    # Bytecode generation and backpatching logic
│   ├── lexer.cpp       # Tokenization and source scanning
│   ├── main.cpp        # Entry point: REPL or file runner
│   ├── parser.cpp      # Recursive descent parser
│   └── vm.cpp          # Stack-based execution loop
│
├── build/              # CMake build artifacts (gitignored)
├── CMakeLists.txt
├── .gitignore
└── README.md
```

---

## Prerequisites

- C++17-compatible compiler: GCC 8+, Clang 7+, or MSVC 2017+
- CMake 3.15 or higher

No external libraries or package managers are required.

---

## Building from Source

### Using CMake (recommended)

```bash
# Clone the repository
git clone https://github.com/your-username/cvm-plus-plus.git
cd cvm-plus-plus

# Create an isolated build directory
mkdir build && cd build

# Configure and build
cmake ..
cmake --build .
```

The compiled binary will be at `build/cvm`.

### Using g++ directly

```bash
g++ -std=c++17 -O2 -Iinclude src/*.cpp -o cvm
```

---

## Running CVM++

### Execute a script file

```bash
./build/cvm path/to/script.cvm
```

###  Interactive REPL

```bash
./build/cvm
```

Launches a Read-Eval-Print Loop for interactive expression evaluation.

---

## Debug Mode

Pass the `--debug` flag to print the generated AST and compiled bytecode before execution. Useful for understanding the compilation pipeline on any given input.

```bash
./build/cvm path/to/script.cvm --debug
```

Example debug output:

```
=== AST ===
BinaryExpr (+)
  NumberLiteral (3)
  NumberLiteral (5)

=== Bytecode ===
0000  OP_CONSTANT  3
0002  OP_CONSTANT  5
0004  OP_ADD
0005  OP_PRINT

=== Execution Result ===
8
```

---

## Instruction Set Architecture

| OpCode             | Description                                      |
| ------------------ | ------------------------------------------------ |
| `OP_CONSTANT`      | Push a constant value onto the stack             |
| `OP_ADD`           | Pop two values, push their sum                   |
| `OP_SUBTRACT`      | Pop two values, push their difference            |
| `OP_MULTIPLY`      | Pop two values, push their product               |
| `OP_DIVIDE`        | Pop two values, push their quotient              |
| `OP_EQUAL`         | Pop two values, push boolean equality result     |
| `OP_LESS`          | Pop two values, push boolean less-than result    |
| `OP_GREATER`       | Pop two values, push boolean greater-than result |
| `OP_SET_VAR`       | Pop value and store in variable map by name      |
| `OP_GET_VAR`       | Push named variable's value onto the stack       |
| `OP_JUMP_IF_FALSE` | If top of stack is false, advance ip by offset   |
| `OP_JUMP`          | Unconditional advance of ip by offset            |
| `OP_PRINT`         | Pop and print the top of stack                   |
| `OP_POP`           | Discard the top of stack                         |

---

## References

- **Crafting Interpreters** — Robert Nystrom (primary architectural reference). Available free at [craftinginterpreters.com](https://craftinginterpreters.com)
- Writing a Lexer in C++
- Understanding Stack-Based Virtual Machines

---
