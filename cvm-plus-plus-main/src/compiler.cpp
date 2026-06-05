#include "compiler.h"

#include <string>

// ==========================================
// BYTECODE WRITERS
// ==========================================
void Compiler::emitByte(uint8_t byte) { chunk.write(byte); }

void Compiler::emitConstant(double value) {
  emitByte(static_cast<uint8_t>(OpCode::OP_CONSTANT));  // Instruction
  int constantIndex = chunk.addConstant(value);  // Save the massive number
  emitByte(static_cast<uint8_t>(
      constantIndex));  // Tell the instruction where to find it
}

// ==========================================
// THE MAIN COMPILER LOOP
// ==========================================
Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>>& statements) {
  for (const auto& stmt : statements) {
    if (stmt != nullptr) {
      compileStmt(stmt.get());
    }
  }
  emitByte(static_cast<uint8_t>(OpCode::OP_RETURN));  // Tell the VM to stop
  return chunk;
}

// ==========================================
// STATEMENT COMPILATION
// ==========================================
void Compiler::compileStmt(Stmt* stmt) {
  if (auto printStmt = dynamic_cast<PrintStmt*>(stmt)) {
    compileExpr(printStmt->expression.get());
    emitByte(static_cast<uint8_t>(OpCode::OP_PRINT));
  } else if (auto varStmt = dynamic_cast<VarStmt*>(stmt)) {
    compileExpr(varStmt->initializer.get());
    uint8_t nameIdx = chunk.addStringConstant(varStmt->name.lexeme);
    emitByte(static_cast<uint8_t>(OpCode::OP_DEFINE_GLOBAL));
    emitByte(nameIdx);
  }
  // --- NEW: CONTROL FLOW COMPILATION ---
  else if (auto blockStmt = dynamic_cast<BlockStmt*>(stmt)) {
    // A block is just a list of statements, compile them in order!
    for (const auto& innerStmt : blockStmt->statements) {
      compileStmt(innerStmt.get());
    }
  } else if (auto ifStmt = dynamic_cast<IfStmt*>(stmt)) {
    compileExpr(ifStmt->condition.get());  // Calculate true/false

    emitByte(static_cast<uint8_t>(OpCode::OP_JUMP_IF_FALSE));
    emitByte(0);  // DUMMY BYTE: We don't know the jump distance yet!
    int thenJumpOffset =
        chunk.code.size() - 1;  // Remember where the dummy byte is

    compileStmt(ifStmt->thenBranch.get());  // Compile the "if" block

    int elseJumpOffset = -1;
    if (ifStmt->elseBranch != nullptr) {
      emitByte(static_cast<uint8_t>(OpCode::OP_JUMP));
      emitByte(0);  // DUMMY BYTE for skipping the else block
      elseJumpOffset = chunk.code.size() - 1;
    }

    // BACKPATCHING: Now we know exactly how big the 'then' block is. Go back
    // and fill it in!
    chunk.code[thenJumpOffset] = chunk.code.size() - thenJumpOffset - 1;

    if (ifStmt->elseBranch != nullptr) {
      compileStmt(ifStmt->elseBranch.get());  // Compile the "else" block
      // Backpatch the else jump
      chunk.code[elseJumpOffset] = chunk.code.size() - elseJumpOffset - 1;
    }
  } else if (auto whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
    int loopStart = chunk.code.size();  // Remember where the loop begins

    compileExpr(whileStmt->condition.get());  // Calculate true/false

    emitByte(static_cast<uint8_t>(OpCode::OP_JUMP_IF_FALSE));
    emitByte(0);  // DUMMY BYTE for exiting the loop
    int exitJumpOffset = chunk.code.size() - 1;

    compileStmt(whileStmt->body.get());  // Compile the inside of the loop

    // Emit the command to jump backwards to 'loopStart'
    emitByte(static_cast<uint8_t>(OpCode::OP_LOOP));
    int loopDistance = chunk.code.size() - loopStart + 1;
    emitByte(static_cast<uint8_t>(loopDistance));

    // BACKPATCHING: Fill in the exit jump distance so it knows how to break the
    // loop
    chunk.code[exitJumpOffset] = chunk.code.size() - exitJumpOffset - 1;
  }
}

// ==========================================
// EXPRESSION COMPILATION
// ==========================================
void Compiler::compileExpr(Expr* expr) {
  if (auto literal = dynamic_cast<LiteralExpr*>(expr)) {
    double val = std::stod(literal->value.lexeme);
    emitConstant(val);
  } else if (auto varExpr = dynamic_cast<VariableExpr*>(expr)) {
    // NEW: Handle reading variables!
    uint8_t nameIdx = chunk.addStringConstant(varExpr->name.lexeme);
    emitByte(static_cast<uint8_t>(OpCode::OP_GET_GLOBAL));
    emitByte(nameIdx);
  } else if (auto binary = dynamic_cast<BinaryExpr*>(expr)) {
    compileExpr(binary->left.get());
    compileExpr(binary->right.get());

    switch (binary->op.type) {
      case TokenType::PLUS:
        emitByte(static_cast<uint8_t>(OpCode::OP_ADD));
        break;
      case TokenType::MINUS:
        emitByte(static_cast<uint8_t>(OpCode::OP_SUBTRACT));
        break;
      case TokenType::STAR:
        emitByte(static_cast<uint8_t>(OpCode::OP_MULTIPLY));
        break;
      case TokenType::SLASH:
        emitByte(static_cast<uint8_t>(OpCode::OP_DIVIDE));
        break;
      case TokenType::EQUAL_EQUAL:
        emitByte(static_cast<uint8_t>(OpCode::OP_EQUAL));
        break;
      case TokenType::LESS:
        emitByte(static_cast<uint8_t>(OpCode::OP_LESS));
        break;
      case TokenType::GREATER:
        emitByte(static_cast<uint8_t>(OpCode::OP_GREATER));
        break;
      default:
        break;
    }
  }
}