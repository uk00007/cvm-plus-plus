#include <iostream>
#include <string>
#include <vector>

#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "vm.h"

int main() {
  // A script that tests if, blocks, and a while loop countdown!
  std::string sourceCode = R"(
        let a = 20;
let b = 7;

print a + b;  
print a - b;    
print a * b;   

let input = 0;

if (input == 1) {
    print 1;
} else {
    print 0;
}

let countdown = 5;

if (countdown > 0) {
    print 999;
} else {
    print 0;
}

while (countdown > 0) {
    print countdown;
    let countdown = countdown - 1;
}
    )";

  std::cout << "--- CVM++ Execution Engine --- \n";

  Lexer lexer(sourceCode);
  std::vector<Token> tokens = lexer.scanTokens();

  Parser parser(tokens);
  std::vector<std::unique_ptr<Stmt>> statements = parser.parse();

  Compiler compiler;
  Chunk chunk = compiler.compile(statements);

  VM vm;
  vm.interpret(&chunk);

  return 0;
}