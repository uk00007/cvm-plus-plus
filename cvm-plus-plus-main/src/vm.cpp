#include "vm.h"

#include <iostream>

void VM::push(double value) { stack.push_back(value); }

double VM::pop() {
  double value = stack.back();
  stack.pop_back();
  return value;
}

void VM::interpret(Chunk* targetChunk) {
  chunk = targetChunk;
  ip = 0;  // Start at the very first byte
  run();
}

void VM::run() {
  // Loop continuously until we hit an OP_RETURN
  while (true) {
    // Read the current byte, then move the pointer forward by 1
    uint8_t instruction = chunk->code[ip++];

    switch (static_cast<OpCode>(instruction)) {
      case OpCode::OP_CONSTANT: {
        // The byte immediately after OP_CONSTANT is the index of our number
        uint8_t constantIndex = chunk->code[ip++];
        double constant = chunk->constants[constantIndex];
        push(constant);
        break;
      }
      case OpCode::OP_ADD: {
        double b = pop();  // Right side of the math
        double a = pop();  // Left side of the math
        push(a + b);
        break;
      }
      case OpCode::OP_SUBTRACT: {
        double b = pop();
        double a = pop();
        push(a - b);
        break;
      }
      case OpCode::OP_MULTIPLY: {
        double b = pop();
        double a = pop();
        push(a * b);
        break;
      }
      case OpCode::OP_DIVIDE: {
        double b = pop();
        double a = pop();
        push(a / b);
        break;
      }
      case OpCode::OP_EQUAL: {
        double b = pop();
        double a = pop();
        push(a == b ? 1.0 : 0.0);
        break;
      }
      case OpCode::OP_LESS: {
        double b = pop();
        double a = pop();
        push(a < b ? 1.0 : 0.0);
        break;
      }
      case OpCode::OP_DEFINE_GLOBAL: {
        uint8_t nameIdx = chunk->code[ip++];
        std::string name = chunk->stringConstants[nameIdx];
        double value = pop();   // Grab the math result from the stack
        globals[name] = value;  // Store it safely in memory!
        break;
      }
      case OpCode::OP_GET_GLOBAL: {
        uint8_t nameIdx = chunk->code[ip++];
        std::string name = chunk->stringConstants[nameIdx];

        // Check if variable actually exists
        if (globals.find(name) == globals.end()) {
          std::cerr << "Runtime Error: Undefined variable '" << name << "'\n";
          return;  // Crash gracefully
        }

        push(globals[name]);  // Load from memory back onto the stack
        break;
      }
      case OpCode::OP_GREATER: {
        double b = pop();
        double a = pop();
        push(a > b ? 1.0 : 0.0);
        break;
      }
      case OpCode::OP_PRINT: {
        // Pop the final result off the stack and show it to the user!
        std::cout << pop() << std::endl;
        break;
      }
      case OpCode::OP_RETURN: {
        return;  // End the program
      }
      case OpCode::OP_JUMP_IF_FALSE: {
        uint8_t offset = chunk->code[ip++];  // Read the jump distance
        // If the top of the stack is 0 (false), fast-forward the ip!
        if (stack.back() == 0.0) {
          ip += offset;
        }
        break;
      }
      case OpCode::OP_JUMP: {
        uint8_t offset = chunk->code[ip++];  // Read the jump distance
        ip += offset;                        // Blindly fast-forward
        break;
      }
      case OpCode::OP_LOOP: {
        uint8_t offset = chunk->code[ip++];  // Read the loop distance
        ip -= offset;                        // Jump backwards!
        break;
      }
    }
  }
}