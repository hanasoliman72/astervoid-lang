#include "implementation/semantic/SemanticAnalyzer.h"
#include "implementation/Scanner/Scanner.h"
#include "implementation/Parser/Parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
    std::string filePath;

    if (argc > 1) {
        // Use provided file path from IDE
        filePath = argv[1];
    } else {
        // Fallback to test file for standalone testing
        filePath = "D:/ClionProjects/Compiler/implementation/scanner/test.astv";
        std::cerr << "DEBUG: Using default test file\n";
    }

    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error: Could not open file: " << filePath << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    try {
        // 1. Scan
        Scanner scanner(source);
        std::vector<Token> tokens = scanner.scanTokens();

        std::cout << "TOKENS:\n";
        for (const auto &token : tokens) {
            std::cout << token.lexeme << " -----> ("
                      << scanner.tokenTypeToString(token.type) << ")\n";
        }
        std::cout << "-----------------------------------------------------------\n";

        // 2. Parse
        std::cout << "Parse Tree:\n";
        Parser parser(tokens);
        auto parseTree = parser.parse();
        if(parseTree) {
            parseTree->printTree();
            std::cout << "-----------------------------------------------------------\n";

            // 3. Semantic
            std::cout << "Semantic Analyzer:\n";
            SemanticAnalyzer semantic_analyzer;
            semantic_analyzer.analyze(parseTree);
        }
        else std::cout << "Parse Tree is null\n";
    } catch (const std::exception& e) {
        std::cerr << "PARSER ERROR: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}