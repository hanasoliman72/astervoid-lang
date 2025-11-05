#include "Parser.h"

// -------------------- Constructor --------------------
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

// -------------------- Entry Point --------------------
void Parser::parse() {
    try {
        program();
        std::cout << "Syntax OK!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Syntax Error: " << e.what() << std::endl;
    }
}

// -------------------- Grammar Rules --------------------
void Parser::program() {
    launchDirective();
    mainFunction();
    if (!isAtEnd()) {
        throw std::runtime_error("Syntax Error: Expected end of file at line " + std::to_string(peek().line));
    }
}

void Parser::launchDirective() {
    consume(TokenType::LAUNCH, "Expected 'launch' directive at start");
}

void Parser::mainFunction() {
    consume(TokenType::VACUUM, "Expected 'vacuum' for entry function");
    consume(TokenType::MILKYWAY, "Expected 'milkyway' as entry function name");
    consume(TokenType::LEFT_PAREN, "Expected '(' after milkyway");
    consume(TokenType::RIGHT_PAREN, "Expected ')' after milkyway");
    consume(TokenType::LEFT_BRACE, "Expected '{' at start of main function body");
    statementList();
    consume(TokenType::RIGHT_BRACE, "Expected '}' at end of main function body");
}

void Parser::statementList() {
    while (!isAtEnd() && !check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        statement();
    }
}

void Parser::statement() {
    if (check(TokenType::MASS) || check(TokenType::FLUX) || check(TokenType::QUANTUM) ||
        check(TokenType::NEBULA) || check(TokenType::STAR) || check(TokenType::TRUTH) || check(TokenType::VACUUM)) {
        declaration();
    }
    else if (check(TokenType::IDENTIFIER)) {
        assignment();
    }
    else if (check(TokenType::PHASE)) phaseStmt();
    else if (check(TokenType::ORBIT)) orbitStmt();
    else if (check(TokenType::ROTATE)) rotateStmt();
    else if (check(TokenType::SUPERNOVA)) supernovaStmt();
    else if (check(TokenType::SHINE)) outputStmt();
    else if (check(TokenType::MOON)) inputStmt();
    else if (check(TokenType::DARKMATTER)) darkMatterStmt();
    else if (check(TokenType::WARP)) warpStmt();
    else {
        throw std::runtime_error("Unexpected token: " + peek().lexeme);
    }
}

// -------------------- Statements --------------------
void Parser::declaration() {
    advance(); // Type
    consume(TokenType::IDENTIFIER, "Expected identifier after type");
    declarationTail();
    consume(TokenType::SEMICOLON, "Expected ';' after declaration");
}

void Parser::declarationTail() {
    if (match(TokenType::EQUAL)) { // Optional
        expr();
    }
}

void Parser::assignment() {
    advance(); // Identifier
    consume(TokenType::EQUAL, "Expected '=' in assignment");
    expr();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
}

void Parser::phaseStmt() {
    consume(TokenType::PHASE, "Expected 'phase'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'phase'");
    condition();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    consume(TokenType::LEFT_BRACE, "Expected '{' for phase block");
    statementList();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after phase block");
    if (match(TokenType::ECLIPSE)) {
        consume(TokenType::LEFT_BRACE, "Expected '{' for eclipse block");
        statementList();
        consume(TokenType::RIGHT_BRACE, "Expected '}' after eclipse block");
    }
}

void Parser::orbitStmt() {
    consume(TokenType::ORBIT, "Expected 'orbit'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'orbit'");
    condition();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    consume(TokenType::LEFT_BRACE, "Expected '{' for orbit block");
    statementList();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after orbit block");
}

void Parser::rotateStmt() {
    consume(TokenType::ROTATE, "Expected 'rotate'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'rotate'");

    forInit();
    consume(TokenType::SEMICOLON, "Expected ';' in rotate");

    conditionOpt();
    consume(TokenType::SEMICOLON, "Expected ';' in rotate");

    assignmentOpt();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after rotate header");

    consume(TokenType::LEFT_BRACE, "Expected '{' for rotate block");
    statementList();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after rotate block");
}

void Parser::forInit() {
    if (check(TokenType::MASS) || check(TokenType::FLUX) || check(TokenType::QUANTUM) ||
        check(TokenType::NEBULA) || check(TokenType::STAR) || check(TokenType::TRUTH) || check(TokenType::VACUUM)) {
        advance(); // Type
        consume(TokenType::IDENTIFIER, "Expected identifier in for-init");
        if (match(TokenType::EQUAL)) expr(); // Optional initializer
    }
    else if (check(TokenType::IDENTIFIER)) {
        advance(); // Variable
        consume(TokenType::EQUAL, "Expected '=' in assignment for for-init");
        expr();
    }
}

void Parser::conditionOpt() {
    if (!check(TokenType::SEMICOLON)) condition();
}

void Parser::assignmentOpt() {
    if (!check(TokenType::RIGHT_PAREN)) {
        advance(); // Identifier
        consume(TokenType::EQUAL, "Expected '=' in assignment");
        expr();
    }
}

void Parser::supernovaStmt() {
    consume(TokenType::SUPERNOVA, "Expected 'supernova'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'supernova'");
    expr();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
    consume(TokenType::LEFT_BRACE, "Expected '{' for supernova block");

    while (check(TokenType::STARPATH) || check(TokenType::BLACKVOID)) {
        if (check(TokenType::STARPATH)) starPath();
        else if (check(TokenType::BLACKVOID)) blackVoidOpt();
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after supernova block");
}

void Parser::starPathList() {
    while (check(TokenType::STARPATH)) starPath();
}

void Parser::starPath() {
    consume(TokenType::STARPATH, "Expected 'starPath'");
    consume(TokenType::NUMBER, "Expected number after 'starPath'");
    consume(TokenType::COLON, "Expected ':' after starPath number");

    while (check(TokenType::SHINE) || check(TokenType::DARKMATTER)) {
        if (check(TokenType::SHINE)) outputStmt();
        else if (check(TokenType::DARKMATTER)) darkMatterStmt();
    }
}

void Parser::blackVoidOpt() {
    if (match(TokenType::BLACKVOID)) {
        consume(TokenType::COLON, "Expected ':' after blackVoid");

        while (check(TokenType::SHINE) || check(TokenType::DARKMATTER)) {
            if (check(TokenType::SHINE)) outputStmt();
            else if (check(TokenType::DARKMATTER)) darkMatterStmt();
        }
    }
}

void Parser::outputStmt() {
    consume(TokenType::SHINE, "Expected 'shine'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after shine");
    expr();
    while (match(TokenType::COMMA)) expr();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after shine");
    consume(TokenType::SEMICOLON, "Expected ';' after shine statement");
}

void Parser::inputStmt() {
    consume(TokenType::MOON, "Expected 'moon'");
    consume(TokenType::LEFT_PAREN, "Expected '(' after moon");
    consume(TokenType::IDENTIFIER, "Expected variable name in moon");
    consume(TokenType::RIGHT_PAREN, "Expected ')' after moon");
    consume(TokenType::SEMICOLON, "Expected ';' after moon statement");
}


void Parser::darkMatterStmt() {
    consume(TokenType::DARKMATTER, "Expected 'darkMatter'");
    consume(TokenType::SEMICOLON, "Expected ';' after darkMatter");
}

void Parser::warpStmt() {
    consume(TokenType::WARP, "Expected 'warp'");
    consume(TokenType::SEMICOLON, "Expected ';' after warp");
}

// -------------------- Expressions --------------------
void Parser::condition() {
    expr();
    if (check(TokenType::EQUAL_EQ) || check(TokenType::BANG_EQ) ||
        check(TokenType::LESS) || check(TokenType::LESS_EQ) ||
        check(TokenType::GREATER) || check(TokenType::GREATER_EQ)) {
        advance();
    } else throw std::runtime_error("Expected relational operator in condition");
    expr();
}

void Parser::expr() {
    term();
    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        advance();
        term();
    }
}

void Parser::term() {
    factor();
    while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
        advance();
        factor();
    }
}

void Parser::factor() {
    if (match(TokenType::NUMBER) || match(TokenType::IDENTIFIER) || match(TokenType::STAR) ||
        match(TokenType::STARLIGHT) || match(TokenType::VOIDNESS)) return;
    if (match(TokenType::LEFT_PAREN)) {
        expr();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return;
    }
    throw std::runtime_error("Expected expression factor, got: " + peek().lexeme);
}

// -------------------- Helpers --------------------
const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

void Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        advance();
    } else {
        throw std::runtime_error(errorMessage + " at line " + std::to_string(peek().line));
    }
}