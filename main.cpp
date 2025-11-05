#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "implementation/Scanner/Scanner.h"
#include "implementation/Parser/Parser.h"

int main() {
    std::ifstream file("D:/ClionProjects/Compiler/implementation/scanner/test.astv");
    if (!file) {
        std::cerr << "Error: Could not open file!\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        // 1. Scan
        Scanner scanner(source);
        std::vector<Token> tokens = scanner.scanTokens();

        // std::cout << "TOKENS:\n";
        // for (const auto &token : tokens) {
        //     std::cout << token.lexeme << " -----> ("
        //               << scanner.tokenTypeToString(token.type) << ")\n";
        // }

        // 2. Parse
        Parser parser(tokens);
        parser.parse();

    } catch (const std::exception& e) {
        std::cerr << "PARSER ERROR: " << e.what() << std::endl;
    }

    return 0;
}
