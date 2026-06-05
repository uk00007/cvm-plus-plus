#ifndef CHUNK_H
#define CHUNK_H

#include <cstdint>
#include <string>
#include <vector>

enum class OpCode : uint8_t {
  OP_CONSTANT,
  OP_ADD,
  OP_SUBTRACT,
  OP_MULTIPLY,
  OP_DIVIDE,
  OP_EQUAL,
  OP_LESS,
  OP_GREATER,
  OP_PRINT,
  OP_RETURN,
  OP_DEFINE_GLOBAL,
  OP_GET_GLOBAL,

  // NEW: Control Flow Commands
  OP_JUMP_IF_FALSE,  // Skip forward if the condition is false
  OP_JUMP,           // Jump (used to loop back or skip the 'else' block)
  OP_LOOP
};

struct Chunk {
  std::vector<uint8_t> code;
  std::vector<double> constants;
  std::vector<std::string> stringConstants;

  void write(uint8_t byte) { code.push_back(byte); }

  int addConstant(double value) {
    constants.push_back(value);
    return constants.size() - 1;
  }

  int addStringConstant(const std::string& str) {
    stringConstants.push_back(str);
    return stringConstants.size() - 1;
  }
};

#endif