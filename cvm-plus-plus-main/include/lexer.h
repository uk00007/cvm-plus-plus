#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

#include "token.h"

class Lexer {
 private:
  std::string source;
  std::vector<Token> tokens;

  // State trackers
  int start = 0;    // Start of the current token being scanned
  int current = 0;  // Current character being looked at
  int line = 1;     // What line of code we are on

  // Helper functions (We will write these next)
  bool isAtEnd();
  char advance();
  void addToken(TokenType type);
  void scanToken();

 public:
  Lexer(std::string source) : source(source) {}

  std::vector<Token> scanTokens();
};

#endif