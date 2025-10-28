#include "Scanner.h"
#include <unordered_map>
#include <iostream>

Scanner::Scanner(const std::string& sourceCode) : source(sourceCode) {}

std::vector<Token> Scanner::scanTokens() {
    while(!isAtEnd()) {
        skipWhitespace();
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

char Scanner::advance() { // GIVE ME THE CURRENT CHAR AND MOVE FORWARD
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
        case '.': return makeToken(TokenType::DOT, ".");
        case ';': return makeToken(TokenType::SEMICOLON, ";");
        case ':': return makeToken(TokenType::COLON, ":");

        // ONE OR TWO CHAR TOKENS
        case '+':
            if(match('+')) return makeToken(TokenType::PLUS_PLUS, "++");
            if(match('=')) return makeToken(TokenType::PLUS_EQ, "+=");
            return makeToken(TokenType::PLUS, "+");
        case '-':
            if(match('-')) return makeToken(TokenType::MINUS_MINUS, "--");
            if(match('=')) return makeToken(TokenType::MINUS_EQ, "-=");
            return makeToken(TokenType::MINUS, "-");
        case '*':
            if(match('=')) return makeToken(TokenType::STARR_EQ, "*=");
            return makeToken(TokenType::STARR, "*");
        case '/':
            if(match('/')) { // SINGLE-LINE COMMENT
                while(peek() != '\n' && !isAtEnd()) advance();
                return makeToken(TokenType::COMMENT, "COMMENT"); // skip
            }
            if(match('*')) { // MULTI-LINE COMMENT
                while(!isAtEnd() && !(peek() == '*' && peekNext() == '/')) {
                    if(peek() == '\n') { line++; col = 1; }
                    advance();
                }
                if(!isAtEnd()) { advance(); advance(); }
                return makeToken(TokenType::BLOCK_COMMENT, "BLOCK_COMMENT");
            }
            if(match('=')) {
                return makeToken(TokenType::SLASH_EQ, "/=");
            }
            return makeToken(TokenType::SLASH, "/");
        case '=':
            if(match('=')) return makeToken(TokenType::EQUAL_EQ, "=");
            return makeToken(TokenType::EQUAL, "=");
        case '>':
            if(match('=')) return makeToken(TokenType::GREATER_EQ, ">=");
            if(current >= 0 && source[current - 1] == 'g') return makeToken(TokenType::ARROW, "<");
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

        // case ' ':
        // case '\r':
        // case '\t':
        //     advance();
        case '\n':
            return makeToken(TokenType::NEW_LINE, "\n");
        default:
            if(isDigit(ch)) return number();
            if(isAlpha(ch)) return identifier();
            return makeToken(TokenType::ERROR, std::string(1, ch));
    }
}

Token Scanner::stringLiteral() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') { line++; col = 1; }
        advance();
    }

    if (isAtEnd()) return makeToken(TokenType::ERROR, "Unterminated string.");

    // LOSING QUOTE
    advance();

    std::string value = source.substr(start + 1, current - start - 2);
    return makeToken(TokenType::STAR, value, value);
}

Token Scanner::number() {
    while (isDigit(peek())) advance();

    // LOOK FOR FRATIONAL PART
    if (peek() == '.' && isDigit(peekNext())) {
        advance(); // ONSUME '.'
        while (isDigit(peek())) advance();
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
        {"quantum", TokenType::QUANTAM},
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
    while (!isAtEnd()) {
        char ch = peek();
        switch(ch) {
            case ' ':
            case '\r':
            case '\t':
                advance();
            break;

            default:
                return;  // STOP SKIPPING
        }
    }
}
