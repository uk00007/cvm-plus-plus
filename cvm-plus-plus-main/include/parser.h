#ifndef PARSER_H
#define PARSER_H

#include <memory>
#include <vector>

#include "ast.h"
#include "token.h"

class Parser {
 private:
  std::vector<Token> tokens;
  int current = 0;

  // --- Grammar Rules ---
  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> varDeclaration();
  std::unique_ptr<Stmt> statement();
  std::unique_ptr<Stmt> printStatement();
  std::unique_ptr<Stmt> ifStatement();
  std::unique_ptr<Stmt> whileStatement();
  std::vector<std::unique_ptr<Stmt>> block();

  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> equality();
  std::unique_ptr<Expr> comparison();
  std::unique_ptr<Expr> term();
  std::unique_ptr<Expr> factor();
  std::unique_ptr<Expr> primary();

  // --- Typewriter Helpers (Similar to Lexer) ---
  bool match(std::initializer_list<TokenType> types);
  bool check(TokenType type);
  Token advance();
  bool isAtEnd();
  Token peek();
  Token previous();
  Token consume(TokenType type, std::string message);

 public:
  Parser(std::vector<Token> tokens) : tokens(tokens) {}

  // The main engine that kicks off the tree building
  std::vector<std::unique_ptr<Stmt>> parse();
};

#endif