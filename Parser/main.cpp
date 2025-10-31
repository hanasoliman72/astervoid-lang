#include "Parser.h"
#include <iostream>

int main() {
    std::vector<Token> tokens = {
        {TokenType::VACUUM, "vacuum", 1},
        {TokenType::IDENTIFIER, "x", 1},
        {TokenType::EQUAL, "=", 1},
        {TokenType::NUMBER, "5", 1},
        {TokenType::SEMICOLON, ";", 1},
        {TokenType::END_OF_FILE, "", 1}
    };

    try {
        Parser parser(tokens);
        auto program = parser.parseProgram();
        std::cout << "✅ Parsing successful!\n";
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << "\n";
    }
}
