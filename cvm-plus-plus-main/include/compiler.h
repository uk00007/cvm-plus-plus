#ifndef COMPILER_H
#define COMPILER_H

#include <memory>
#include <vector>

#include "ast.h"
#include "chunk.h"

class Compiler {
 private:
  Chunk chunk;

  // Recursive functions to walk down the AST
  void compileExpr(Expr* expr);
  void compileStmt(Stmt* stmt);

  // Helpers to emit raw bytes into our Chunk
  void emitByte(uint8_t byte);
  void emitConstant(double value);

 public:
  // Takes the tree from the Parser and returns a flattened Chunk of bytecode
  Chunk compile(const std::vector<std::unique_ptr<Stmt>>& statements);
};

#endif