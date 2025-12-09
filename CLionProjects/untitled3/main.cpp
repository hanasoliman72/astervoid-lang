#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "astervoid-lang/implementation/Scanner/Scanner.h"
#include "astervoid-lang/implementation/Parser/Parser.h"
#include "astervoid-lang/implementation/Semantic/SemanticAnalyzer.h"

int main() {
    std::ifstream file("C:/Users/EES/CLionProjects/untitled3/astervoid-lang/implementation/scanner/test.astv");
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

        std::cout << "=== LEXICAL ANALYSIS ===\n";
        std::cout << "TOKENS:\n";
        for (const auto &token : tokens) {
            std::cout << token.lexeme << " -----> ("
                      << scanner.tokenTypeToString(token.type) << ")\n";
        }
        std::cout << "\n";

        // 2. Parse
        std::cout << "=== SYNTAX ANALYSIS ===\n";
        Parser parser(tokens);
        auto parseTree = parser.parse();

        if(parseTree) {
            std::cout << "\nParse Tree:\n";
            parseTree->printTree();
            std::cout << "\n";

            // 3. Semantic Analysis
            std::cout << "=== SEMANTIC ANALYSIS ===\n";
            SemanticAnalyzer semanticAnalyzer;
            bool semanticSuccess = semanticAnalyzer.analyze(parseTree);

            if (semanticSuccess) {
                std::cout << "\n=== COMPILATION SUCCESSFUL ===\n";
                std::cout << "All phases completed without errors.\n";
            } else {
                std::cout << "\n=== COMPILATION FAILED ===\n";
                std::cout << "Semantic errors detected.\n";
                return 1;
            }
        }
        else {
            std::cout << "Parse Tree is null - syntax errors present\n";
            return 1;
        }
    } catch (const std::exception& e) {
        std::cerr << "COMPILER ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}