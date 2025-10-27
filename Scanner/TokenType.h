#ifndef TOKENTYPE_H
#define TOKENTYPE_H

#include <string>

enum class TokenType {
    // SINGLE CHAR
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    COMMA, DOT, SEMICOLON, COLON,

    // ONE OR TWO CHAR TOKENS
    PLUS, PLUS_PLUS, PLUS_EQ,
    MINUS, MINUS_MINUS, MINUS_EQ, ARROW,
    STARR, STARR_EQ,
    SLASH, SLASH_EQ,
    PERCENT, BANG, BANG_EQ,
    EQUAL, EQUAL_EQ,
    GREATER, GREATER_EQ,
    LESS, LESS_EQ,
    AND, OR, XOR,

    // LITERALS
    IDENTIFIER, STAR, NUMBER,

    // KEYWORDS
    LAUNCH, MASS, FLUX, QUANTAM, NEBULA, TRUTH, STARLIGHT,
    VOIDNESS, VACUUM, BLACKHOLE, DARKMATTER, WARP, ROTATE,
    ORBIT, PHASE, ECLIPSE, SUPERNOVA, STARPATH, BLACKVOID,
    MOON, SHINE, GALAXY, EARTH, CONSTELLATION, CONSTRUCT,
    DEORBIT, SHIELD, RECOVER, EJECT, OPEN, CORE, ORBITSHIELD,

    // SPECIAL
    END_OF_FILE,
    ERROR
};

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
        case TokenType::PLUS: return "PLUS";
        case TokenType::PLUS_PLUS: return "PLUS_PLUS";
        case TokenType::PLUS_EQ: return "PLUS_EQ";
        case TokenType::MINUS: return "MINUS";
        case TokenType::MINUS_MINUS: return "MINUS_MINUS";
        case TokenType::MINUS_EQ: return "MINUS_EQ";
        case TokenType::ARROW: return "ARROW";
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
        case TokenType::ERROR: return "ERROR";

        default:
            return "UNKNOWN";
    }
}

#endif
