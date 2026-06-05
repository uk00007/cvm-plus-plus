// this file will define our vocab, what words and symbols does CVM++ acutally
// understand?

#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
  LEFT_PAREN,
  RIGHT_PAREN,
  LEFT_BRACE,
  RIGHT_BRACE,
  SEMICOLON,
  PLUS,
  MINUS,
  START,
  STAR,
  SLASH,

  EQUAL,
  EQUAL_EQUAL,
  LESS,
  LESS_EQUAL,
  GREATER,
  GREATER_EQUAL,

  IDENTIFIER,
  NUMBER,

  LET,
  PRINT,
  IF,
  ELSE,
  WHILE,
  TRUE_TOKEN,
  FALSE_TOKEN,

  TOKEN_EOF
};

struct Token {
  TokenType type;
  std::string lexeme;
  int line;

  Token(TokenType type, std::string lexeme, int line)
      : type(type), lexeme(lexeme), line(line) {}
};

#endif