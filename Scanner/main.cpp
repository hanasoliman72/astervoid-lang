#include <bits/stdc++.h>
#include <iostream>

#include "Scanner.h"

using namespace std;

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        // SINGLE CHAR
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::COLON: return "COLON";

        // ONE OR TWO CHAR TOKENS
        case TokenType::ARROW: return "ARROW";
        case TokenType::PLUS: return "PLUS";
        case TokenType::PLUS_PLUS: return "PLUS_PLUS";
        case TokenType::PLUS_EQ: return "PLUS_EQ";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MINUS_MINUS: return "MINUS_MINUS";
        case TokenType::MINUS_EQ: return "MINUS_EQ";
        case TokenType::STARR: return "STARR";
        case TokenType::STARR_EQ: return "STARR_EQ";
        case TokenType::SLASH: return "SLASH";
        case TokenType::SLASH_EQ: return "SLASH_EQ";
        case TokenType::PERCENT: return "PERCENT";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQ: return "BANG_EQ";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQ: return "EQUAL_EQ";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQ: return "GREATER_EQ";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQ: return "LESS_EQ";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::XOR: return "XOR";

        // LITERALS
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STAR: return "STAR";
        case TokenType::NUMBER: return "NUMBER";

        // KEYWORDS
        case TokenType::LAUNCH: return "LAUNCH";
        case TokenType::MASS: return "MASS";
        case TokenType::BIGBANG: return "BIGBANG";
        case TokenType::MILKYWAY: return "MILKYWAY";
        case TokenType::FLUX: return "FLUX";
        case TokenType::QUANTAM: return "QUANTAM";
        case TokenType::NEBULA: return "NEBULA";
        case TokenType::TRUTH: return "TRUTH";
        case TokenType::STARLIGHT: return "STARLIGHT";
        case TokenType::VOIDNESS: return "VOIDNESS";
        case TokenType::VACUUM: return "VACUUM";
        case TokenType::BLACKHOLE: return "BLACKHOLE";
        case TokenType::DARKMATTER: return "DARKMATTER";
        case TokenType::WARP: return "WARP";
        case TokenType::ROTATE: return "ROTATE";
        case TokenType::ORBIT: return "ORBIT";
        case TokenType::PHASE: return "PHASE";
        case TokenType::ECLIPSE: return "ECLIPSE";
        case TokenType::SUPERNOVA: return "SUPERNOVA";
        case TokenType::STARPATH: return "STARPATH";
        case TokenType::BLACKVOID: return "BLACKVOID";
        case TokenType::MOON: return "MOON";
        case TokenType::SHINE: return "SHINE";
        case TokenType::GALAXY: return "GALAXY";
        case TokenType::EARTH: return "EARTH";
        case TokenType::CONSTELLATION: return "CONSTELLATION";
        case TokenType::CONSTRUCT: return "CONSTRUCT";
        case TokenType::DEORBIT: return "DEORBIT";
        case TokenType::SHIELD: return "SHIELD";
        case TokenType::RECOVER: return "RECOVER";
        case TokenType::EJECT: return "EJECT";
        case TokenType::OPEN: return "OPEN";
        case TokenType::CORE: return "CORE";
        case TokenType::ORBITSHIELD: return "ORBITSHIELD";

        // SPECIAL
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::NEW_LINE: return "NEW_LINE";
        case TokenType::ERROR: return "ERROR";

        default:
            return "UNKNOWN";
    }
}

int main() {

    // === Read source file ===
    std::ifstream file("D:/ClionProjects/CompilerProject/Scanner/test.aster");
    if (!file) {
        std::cerr << "Error: Could not open file!\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Scanner scanner(source);
    std::vector<Token> tokens = scanner.scanTokens();
    for (const auto &token : tokens) {
        std::cout << token.lexeme << "-----> (" << tokenTypeToString(token.type) << ")\n";
    }
    return 0;
}