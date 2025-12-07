#include "Scanner.h"
#include <unordered_map>
#include <iostream>

Scanner::Scanner(const std::string& sourceCode) : source(sourceCode) {}

std::vector<Token> Scanner::scanTokens() {
    while(!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        start = current;
        startCol = col;
        Token token = scanToken();
        tokens.push_back(token);
    }
    tokens.push_back(makeToken(TokenType::END_OF_FILE, "", ""));
    return tokens;
}

bool Scanner::isAtEnd() const {
    return current >= source.length();
}

// GIVE ME THE CURRENT CHAR AND MOVE FORWARD
char Scanner::advance() {
    char ch =  source[current++];
    if(ch == '\n') line++, col = 1;
    else col++;
    return ch;
}

char Scanner::peek() const {
    if(isAtEnd()) return '\0';
    return source[current];
}

char Scanner::peekNext() const {
    if(current + 1 >= source.length()) return '\0';
    return source[current + 1];
}

char Scanner::peekThird() const{
    if (current + 2 >= source.length()) return '\0';
    return source[current + 2];
}

// CHECK WHETHER THE NEXT CHAR IN THE SOURCE MATCHES
// A SPECIFIC ONE AND ADVANCE IF IT DOES
bool Scanner::match(char expected) {
    if(isAtEnd()) return false;
    if(source[current] != expected) return false;
    current++, col++;
    return true;
}

Token Scanner::makeToken(TokenType type, const std::string &lexeme, const std::string &literal) {
    return Token{type, lexeme, literal,line,col};
}

Token Scanner::scanToken() {
    char ch = advance();

    switch(ch) {
        // SINGLE CHAR
        case '(': return makeToken(TokenType::LEFT_PAREN, "(");
        case ')': return makeToken(TokenType::RIGHT_PAREN, ")");
        case '{': return makeToken(TokenType::LEFT_BRACE, "{");
        case '}': return makeToken(TokenType::RIGHT_BRACE, "}");
        case ',': return makeToken(TokenType::COMMA, ",");
        case '.':
            if(isDigit(peek())) return number();
            return makeToken(TokenType::DOT, ".");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case ':': return makeToken(TokenType::COLON, ":");
        case '#': return directive();

        // ONE OR TWO CHAR TOKENS
        case '+':
            if(match('+')) return makeToken(TokenType::PLUS_PLUS, "++");
            if(match('=')) return makeToken(TokenType::PLUS_EQ, "+=");
            return makeToken(TokenType::PLUS, "+");
        case '-':
            if(match('-')) return makeToken(TokenType::MINUS_MINUS, "--");
            if(match('=')) return makeToken(TokenType::MINUS_EQ, "-=");
            return makeToken(TokenType::MINUS, "-");
        case '*': return comment();
        case '/':
            if(match('=')) {
                return makeToken(TokenType::SLASH_EQ, "/=");
            }
            return makeToken(TokenType::SLASH, "/");
        case '=':
            if(match('=')) return makeToken(TokenType::EQUAL_EQ, "==");
            return makeToken(TokenType::EQUAL, "=");
        case '>':
            if(match('=')) return makeToken(TokenType::GREATER_EQ, ">=");
            return makeToken(TokenType::GREATER, ">");
        case '<':
            if(match('=')) return makeToken(TokenType::LESS_EQ, "<=");
            return makeToken(TokenType::LESS, "<");
        case '%':
            if(match('=')) return makeToken(TokenType::PERCENT_EQ, "%=");
            return makeToken(TokenType::PERCENT, "%");
        case '!':
            if(match('=')) return makeToken(TokenType::BANG_EQ, "!=");
            return makeToken(TokenType::BANG, "!");
        case '&':
            if(match('&')) return makeToken(TokenType::AND, "&&");
            return makeToken(TokenType::BIT_AND, "&");
        case '|':
            if(match('|')) return makeToken(TokenType::OR, "||");
            return makeToken(TokenType::BIT_OR, "|");
        case '^':
            return makeToken(TokenType::XOR, "^");

        // LITERALS
        case '"': return stringLiteral();

        default:
            if(isDigit(ch)) return number();
            if(isAlpha(ch)) return identifier();
            if (std::isspace(ch)) return scanToken();
            return makeToken(TokenType::ERROR, std::string(1, ch));
    }
}

Token Scanner::directive() {
    // THE # IS ALREADY COMSUMED
    int dirStart = start;
    while(!isAtEnd() && (isAlpha(peek()) || peek() == '_')) {
        advance();
    }
    std::string name = source.substr(dirStart, current - dirStart);

    skipWhitespace();
    if(peek() == '<') {
        advance(); // CONSUME <
        int paramStart = current;
        while(!isAtEnd() && peek() != '>') advance();
        std::string parameter = source.substr(paramStart, current - paramStart);
        if(peek() == '>') advance(); // CONSUME >
        return makeToken(TokenType::LAUNCH, name + " <" + parameter + ">");
    }
    return makeToken(TokenType::ERROR, name);
}

Token Scanner::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { line++; col = 1; }
        advance();
    }

    if (isAtEnd()) return makeToken(TokenType::ERROR, "Unterminated string.");

    // LOSING QUOTE
    advance();

    // std::string value = source.substr(start + 1, current - start - 2);
    int len = current - start - 2;
    if (len < 0) len = 0;
    std::string value = source.substr(start + 1, len);
    return makeToken(TokenType::STAR, value, value);
}

Token Scanner::number() {
    // CONSUME DIGITS BEFORE DECIMAL
    while (isDigit(peek())) advance();

    // DECIMAL PART
    if (peek() == '.') {
        // CHECK FOR SECOND DOT
        if (peekNext() == '.') {
            advance(); advance();
            while (!isAtEnd() && !std::isspace(peek()) && peek() != ';' && peek() != ')')
                advance();
            return makeToken(TokenType::ERROR, source.substr(start, current - start));
        }

        advance(); // CONSUME '.'
        // DIGITS AFTER DECIMAL
        while (isDigit(peek())) advance();
    }

    if (isAlpha(peek()) || peek() == '.') {
        while (!isAtEnd() && !std::isspace(peek()) && peek() != ';' && peek() != ')')
            advance();

        return makeToken(TokenType::ERROR, source.substr(start, current - start));
    }

    std::string value = source.substr(start, current - start);
    return makeToken(TokenType::NUMBER, value, value);
}

Token Scanner::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    std::string text = source.substr(start, current - start);
    TokenType type = identifierType(text);
    return makeToken(type, text);
}

Token Scanner::comment() {
    if (match('*')) {
        if(peek() == '*') {
            while (!isAtEnd() && !(peek() == '*' && peekNext() == '*' && peekThird() == '*')) {
                advance();
            }
            advance(),advance(),advance();
            skipWhitespace();
            start = current;
            return scanToken();
        }
        while (!isAtEnd() && peek() != '\n') {
            advance();
        }
        if(peek() == '\n') {
            advance();
            skipWhitespace();
            start = current;
            return scanToken();
        }
    }
    else if(match('=')) {
        return makeToken(TokenType::STARR_EQ, "*=");
    }
    else return makeToken(TokenType::STARR, "*");
}

bool Scanner::isDigit(char ch) const {
    return ch >= '0' && ch <= '9';
}

bool Scanner::isAlpha(char ch) const {
    return (ch >= 'a' && ch <= 'z') ||
           (ch >= 'A' && ch <= 'Z') ||
            ch == '_';
}

TokenType Scanner::identifierType(const std::string &s) {
    static const std::unordered_map<std::string, TokenType> keywords = {
        {"launch", TokenType::LAUNCH},
        {"bigbang", TokenType::BIGBANG},
        {"milkyway", TokenType::MILKYWAY},
        {"mass", TokenType::MASS},
        {"flux", TokenType::FLUX},
        {"quantum", TokenType::QUANTUM},
        {"nebula", TokenType::NEBULA},
        {"star", TokenType::STAR},
        {"truth", TokenType::TRUTH},
        {"starlight", TokenType::TRUTH},   // true
        {"voidness", TokenType::VOIDNESS}, // false
        {"vacuum", TokenType::VACUUM},
        {"blackHole", TokenType::BLACKHOLE},
        {"darkMatter", TokenType::DARKMATTER},
        {"warp", TokenType::WARP},
        {"rotate", TokenType::ROTATE},
        {"orbit", TokenType::ORBIT},
        {"phase", TokenType::PHASE},
        {"eclipse", TokenType::ECLIPSE},
        {"supernova", TokenType::SUPERNOVA},
        {"starPath", TokenType::STARPATH},
        {"blackVoid", TokenType::BLACKVOID},
        {"moon", TokenType::MOON},
        {"shine", TokenType::SHINE},
        {"galaxy", TokenType::GALAXY},
        {"earth", TokenType::EARTH},
        {"constellation", TokenType::CONSTELLATION},
        {"construct", TokenType::CONSTRUCT},
        {"deorbit", TokenType::DEORBIT},
        {"shield", TokenType::SHIELD},
        {"recover", TokenType::RECOVER},
        {"eject", TokenType::EJECT},
        {"open", TokenType::OPEN},
        {"core", TokenType::CORE},
        {"orbitshield", TokenType::ORBITSHIELD}
    };

    auto it = keywords.find(s);
    if (it != keywords.end()) return it->second;
    return TokenType::IDENTIFIER;
}

void Scanner::skipWhitespace() {
    while(!isAtEnd()) {
        char ch = peek();
        if(ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n') {
            advance();
        }
        else break;
    }
}

std::string Scanner::tokenTypeToString(TokenType type) {
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
        case TokenType::HASH: return "HASH";

        // ONE OR TWO CHAR TOKENS
        case TokenType::COMMENT: return "COMMENT";
        case TokenType::BLOCK_COMMENT: return "BLOCK_COMMENT";
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
        case TokenType::BIT_AND: return "BIT_AND";
        case TokenType::OR: return "OR";
        case TokenType::BIT_OR: return "BIT_OR";
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
        case TokenType::QUANTUM: return "QUANTUM";
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