#ifndef AST_H
#define AST_H

#include <memory>
#include <vector>

#include "token.h"

// ==========================================
// 1. EXPRESSIONS (Things that output a value)
// ==========================================

// The parent blueprint for all Expressions
struct Expr {
  virtual ~Expr() = default;  // Allows safe deletion of the tree later
};

// Represents a raw number or boolean (e.g., 10, true)
struct LiteralExpr : public Expr {
  Token value;

  LiteralExpr(Token value) : value(value) {}
};

// Represents math or comparison (e.g., 2 + 3, x < 10)
struct BinaryExpr : public Expr {
  std::unique_ptr<Expr> left;   // The left side of the math
  Token op;                     // The operator (+, -, *, /)
  std::unique_ptr<Expr> right;  // The right side of the math

  BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
      : left(std::move(left)), op(op), right(std::move(right)) {}
};

// Represents looking up a variable (e.g., reading 'x')
struct VariableExpr : public Expr {
  Token name;

  VariableExpr(Token name) : name(name) {}
};

// ==========================================
// 2. STATEMENTS (Things that perform actions)
// ==========================================

// The parent blueprint for all Statements
struct Stmt {
  virtual ~Stmt() = default;
};

// Represents declaring a variable (e.g., let x = 10;)
struct VarStmt : public Stmt {
  Token name;
  std::unique_ptr<Expr>
      initializer;  // The math/value on the right side of the '='

  VarStmt(Token name, std::unique_ptr<Expr> initializer)
      : name(name), initializer(std::move(initializer)) {}
};

// Represents printing to the terminal (e.g., print x + 5;)
struct PrintStmt : public Stmt {
  std::unique_ptr<Expr> expression;

  PrintStmt(std::unique_ptr<Expr> expression)
      : expression(std::move(expression)) {}
};

// Represents a block of code enclosed in { }
struct BlockStmt : public Stmt {
  std::vector<std::unique_ptr<Stmt>> statements;

  BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
      : statements(std::move(statements)) {}
};

// Represents an if-else statement
struct IfStmt : public Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> thenBranch;
  std::unique_ptr<Stmt>
      elseBranch;  // This can be nullptr if there is no 'else'

  IfStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> thenBranch,
         std::unique_ptr<Stmt> elseBranch)
      : condition(std::move(condition)),
        thenBranch(std::move(thenBranch)),
        elseBranch(std::move(elseBranch)) {}
};

// Represents a while loop
struct WhileStmt : public Stmt {
  std::unique_ptr<Expr> condition;
  std::unique_ptr<Stmt> body;

  WhileStmt(std::unique_ptr<Expr> condition, std::unique_ptr<Stmt> body)
      : condition(std::move(condition)), body(std::move(body)) {}
};

#endif