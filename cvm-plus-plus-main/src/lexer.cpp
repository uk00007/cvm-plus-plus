#include "lexer.h"

#include <cctype>  // for isdigit() and isalpha()
#include <iostream>

// 1. THE MAIN ENGINE LOOP
std::vector<Token> Lexer::scanTokens() {
  while (!isAtEnd()) {
    // We are at the beginning of the next lexeme.
    start = current;
    scanToken();
  }

  // Append an End-Of-File token so the Parser knows when to stop
  tokens.push_back(Token(TokenType::TOKEN_EOF, "", line));
  return tokens;
}

// 2. THE TYPEWRITER CONTROLS
bool Lexer::isAtEnd() { return current >= source.length(); }

char Lexer::advance() {
  return source[current++];  // Return the current character, then move forward
                             // 1
}

void Lexer::addToken(TokenType type) {
  std::string text = source.substr(start, current - start);
  tokens.push_back(Token(type, text, line));
}

// 3. THE BRAIN: IDENTIFYING THE WORDS
void Lexer::scanToken() {
  char c = advance();

  switch (c) {
    // Single-character tokens
    case '(':
      addToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      addToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      addToken(TokenType::RIGHT_BRACE);
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case '-':
      addToken(TokenType::MINUS);
      break;
    case '*':
      addToken(TokenType::STAR);
      break;
    case '/':
      addToken(TokenType::SLASH);
      break;

    // One or two character tokens (e.g., checking if '=' is just '=' or '==')
    case '=':
      if (!isAtEnd() && source[current] == '=') {
        current++;  // Consume the second '='
        addToken(TokenType::EQUAL_EQUAL);
      } else {
        addToken(TokenType::EQUAL);
      }
      break;
    case '<':
      if (!isAtEnd() && source[current] == '=') {
        current++;
        addToken(TokenType::LESS_EQUAL);
      } else {
        addToken(TokenType::LESS);
      }
      break;
    case '>':
      if (!isAtEnd() && source[current] == '=') {
        current++;
        addToken(TokenType::GREATER_EQUAL);
      } else {
        addToken(TokenType::GREATER);
      }
      break;

    // Ignore Whitespace
    case ' ':
    case '\r':
    case '\t':
      break;  // Do nothing, just skip
    case '\n':
      line++;  // Track line numbers for error reporting
      break;

    // Multi-character Tokens (Numbers, Variables, Keywords)
    default:
      if (isdigit(c)) {
        // Keep consuming numbers until we hit a space or symbol
        while (!isAtEnd() && isdigit(source[current])) {
          current++;
        }
        addToken(TokenType::NUMBER);
      } else if (isalpha(c)) {
        // Keep consuming letters/numbers for variable names (e.g., "var1")
        while (!isAtEnd() && isalnum(source[current])) {
          current++;
        }

        std::string text = source.substr(start, current - start);

        // Check if it's a reserved keyword, otherwise it's just an identifier
        if (text == "let")
          addToken(TokenType::LET);
        else if (text == "print")
          addToken(TokenType::PRINT);
        else if (text == "if")
          addToken(TokenType::IF);
        else if (text == "else")
          addToken(TokenType::ELSE);
        else if (text == "while")
          addToken(TokenType::WHILE);
        else if (text == "true")
          addToken(TokenType::TRUE_TOKEN);
        else if (text == "false")
          addToken(TokenType::FALSE_TOKEN);
        else
          addToken(TokenType::IDENTIFIER);
      } else {
        std::cout << "Syntax Error at line " << line
                  << ": Unexpected character '" << c << "'\n";
      }
      break;
  }
}