#include "parser.h"

#include <iostream>

// ==========================================
// THE MAIN LOOP
// ==========================================
std::vector<std::unique_ptr<Stmt>> Parser::parse() {
  std::vector<std::unique_ptr<Stmt>> statements;
  while (!isAtEnd()) {
    statements.push_back(declaration());
  }
  return statements;
}

// ==========================================
// STATEMENTS (Actions)
// ==========================================
std::unique_ptr<Stmt> Parser::declaration() {
  if (match({TokenType::LET})) return varDeclaration();
  return statement();
}

std::unique_ptr<Stmt> Parser::varDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
  consume(TokenType::EQUAL, "Expect '=' after variable name.");

  std::unique_ptr<Expr> initializer = expression();

  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return std::make_unique<VarStmt>(name, std::move(initializer));
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match({TokenType::IF})) return ifStatement();
  if (match({TokenType::WHILE})) return whileStatement();
  if (match({TokenType::LEFT_BRACE}))
    return std::make_unique<BlockStmt>(block());
  if (match({TokenType::PRINT})) return printStatement();

  // If it's not a special keyword, assume it's raw math
  std::unique_ptr<Expr> expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return nullptr;
}

std::unique_ptr<Stmt> Parser::printStatement() {
  std::unique_ptr<Expr> value = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return std::make_unique<PrintStmt>(std::move(value));
}

std::unique_ptr<Stmt> Parser::ifStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  std::unique_ptr<Expr> condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  std::unique_ptr<Stmt> thenBranch = statement();
  std::unique_ptr<Stmt> elseBranch = nullptr;

  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch),
                                  std::move(elseBranch));
}

std::unique_ptr<Stmt> Parser::whileStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  std::unique_ptr<Expr> condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");

  std::unique_ptr<Stmt> body = statement();

  return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
}

std::vector<std::unique_ptr<Stmt>> Parser::block() {
  std::vector<std::unique_ptr<Stmt>> statements;

  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

// ==========================================
// EXPRESSIONS (Math & Logic with Order of Operations)
// ==========================================
std::unique_ptr<Expr> Parser::expression() { return equality(); }

std::unique_ptr<Expr> Parser::equality() {
  std::unique_ptr<Expr> expr = comparison();

  while (match({TokenType::EQUAL_EQUAL})) {
    Token op = previous();
    std::unique_ptr<Expr> right = comparison();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

std::unique_ptr<Expr> Parser::comparison() {
  std::unique_ptr<Expr> expr = term();

  while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                TokenType::LESS_EQUAL})) {
    Token op = previous();
    std::unique_ptr<Expr> right = term();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

std::unique_ptr<Expr> Parser::term() {
  std::unique_ptr<Expr> expr = factor();

  while (match({TokenType::MINUS, TokenType::PLUS})) {
    Token op = previous();
    std::unique_ptr<Expr> right = factor();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

std::unique_ptr<Expr> Parser::factor() {
  std::unique_ptr<Expr> expr = primary();

  while (match({TokenType::SLASH, TokenType::STAR})) {
    Token op = previous();
    std::unique_ptr<Expr> right = primary();
    expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
  }
  return expr;
}

std::unique_ptr<Expr> Parser::primary() {
  if (match(
          {TokenType::FALSE_TOKEN, TokenType::TRUE_TOKEN, TokenType::NUMBER})) {
    return std::make_unique<LiteralExpr>(previous());
  }
  if (match({TokenType::IDENTIFIER})) {
    return std::make_unique<VariableExpr>(previous());
  }

  std::cerr << "Syntax Error: Expected an expression." << std::endl;
  return nullptr;
}

// ==========================================
// HELPERS
// ==========================================
bool Parser::match(std::initializer_list<TokenType> types) {
  for (TokenType type : types) {
    if (check(type)) {
      advance();
      return true;
    }
  }
  return false;
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) return false;
  return peek().type == type;
}

Token Parser::advance() {
  if (!isAtEnd()) current++;
  return previous();
}

bool Parser::isAtEnd() { return peek().type == TokenType::TOKEN_EOF; }

Token Parser::peek() { return tokens[current]; }

Token Parser::previous() { return tokens[current - 1]; }

Token Parser::consume(TokenType type, std::string message) {
  if (check(type)) return advance();
  std::cerr << "Syntax Error at line " << peek().line << ": " << message
            << std::endl;
  exit(1);  // Crash the compiler if the user wrote bad syntax!
}