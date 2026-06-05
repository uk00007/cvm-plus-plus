#ifndef VM_H
#define VM_H

#include <string>
#include <unordered_map>
#include <vector>

#include "chunk.h"

class VM {
 private:
  Chunk* chunk;
  int ip;
  std::vector<double> stack;

  // NEW: Our Variable Memory Bank
  std::unordered_map<std::string, double> globals;

  void push(double value);
  double pop();
  void run();

 public:
  VM() : chunk(nullptr), ip(0) {}
  void interpret(Chunk* chunk);
};

#endif