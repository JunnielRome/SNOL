#include <cstring>      // std::strerror
#include <fstream>      // readFile
#include <iostream>     // std::getline
#include <conio.h>      // getch()
#include <string>
#include <vector>
#include "Error.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Scanner.h"

void run(Interpreter interpreter, std::string_view source, bool& hadError) {
  Scanner scanner {source};
  std::vector<Token> tokens = scanner.scanTokens(hadError);

    // for (const Token& token : tokens) {
    // std::cout << token.toString() << "\n";
    // }
  Parser parser{tokens};
  std::vector<std::shared_ptr<Stmt>> statements = parser.parse(hadError);

  // Stop if there was a syntax error.
  if (hadError) return;
  
  interpreter.interpret(statements);
}

void runPrompt() {
  Interpreter interpreter{};
  bool hadError = false;
  bool hadRuntimeError = false;
  std::cout << "The SNOL environment is now active, you may proceed with" << std::endl
      << "giving your commands." << std::endl;
  for (;;) {
    std::cout << "\nCommand: ";
    std::string line;
    if (!std::getline(std::cin, line)) break;
    if (line == "EXIT!") {
    	std::cout << "\nInterpreter is now terminated...";
    	getch();
    	break;
	  }
    run(interpreter, line, hadError);
    hadError = false;
  }
}

int main(int argc, char* argv[]) {
  runPrompt();
}