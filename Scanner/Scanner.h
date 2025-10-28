#ifndef SCANNER_H
#define SCANNER_H

#include "TokenType.h"
#include <string>
#include <vector>

struct Token {
    TokenType type;
    std::string lexeme; // THE EXACT SUBSTRING FROM SOURCE
    std::string literal;// VALUE
    int line;// LINE NUMBER WHERE TOKEN STARTS
    int col;// COLUMN NUMBER WHERE TOKEN STARTS

    std::string toString() const {
        return "[" + std::to_string(line) + ":" + std::to_string(col) + "] " + lexeme;
    }
};

class Scanner {
public:
    Scanner(const std::string &source);
    std::vector<Token> scanTokens();

private:
    std::string source;
    std::vector<Token> tokens;
    int start = 0;// WHERE THE CURRENT TOKEN STARTS
    int current = 0;// WHERE YOU ARE NOW IN THE TEXT
    int line = 1;// CURRENT LINE NUMBER
    int col = 1;// COLUMN OF THE CURRENT CHAR
    int startCol = 1;// COLUMN AT TOKEN START (CURRENT LINE)

    bool isAtEnd() const;
    char advance();
    char peek() const;
    char peekNext() const;
    bool match(char expected);
    Token makeToken(TokenType type, const std::string &lexeme, const std::string &literal = "");
    Token scanToken();
    Token stringLiteral();
    Token number();
    Token identifier();
    bool isDigit(char c) const;
    bool isAlpha(char c) const;
    TokenType identifierType(const std::string &s);
    void skipWhitespace();
};

#endif //SCANNER_H
