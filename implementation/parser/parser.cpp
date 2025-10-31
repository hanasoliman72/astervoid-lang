#include "Parser.h"
#include <stdexcept>
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
const Token& Parser::peek() const { return tokens[current]; }
const Token& Parser::previous() const { return tokens[current - 1]; }
const Token& Parser::advance() { if (!isAtEnd()) current++; return previous(); }
bool Parser::check(TokenType type) const { return !isAtEnd() && peek().type == type; }

bool Parser::match(std::initializer_list<TokenType> types) {
    for (auto type : types) if (check(type)) { advance(); return true; }
    return false;
}

std::vector<std::unique_ptr<Stmt>> Parser::parseProgram() {
    std::vector<std::unique_ptr<Stmt>> stmts;
    while (!isAtEnd()) stmts.push_back(declaration());
    return stmts;
}

std::unique_ptr<Stmt> Parser::declaration() {
    if (match({TokenType::VACUUM, TokenType::MASS, TokenType::FLUX, TokenType::QUANTUM})) {
        // ممكن تبقى function أو variable
        Token type = previous();
        Token name = advance();

        if (match({TokenType::LEFT_PAREN})) {
            // Function
            auto func = std::make_unique<FuncDecl>();
            func->returnType = type;
            func->name = name;
            match({TokenType::RIGHT_PAREN}); // TODO: params later
            func->body.push_back(block());
            return func;
        } else {
            // Variable declaration
            auto var = std::make_unique<VarDecl>();
            var->type = type;
            var->name = name;
            if (match({TokenType::EQUAL})) var->initializer = expression();
            match({TokenType::SEMICOLON});
            return var;
        }
    }

    return statement();
}

std::unique_ptr<Stmt> Parser::block() {
    auto block = std::make_unique<BlockStmt>();
    if (!match({TokenType::LEFT_BRACE})) throw std::runtime_error("Expected '{'");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) block->statements.push_back(declaration());
    match({TokenType::RIGHT_BRACE});
    return block;
}

// Expressions
std::unique_ptr<Expr> Parser::expression() {
    return term();
}

std::unique_ptr<Expr> Parser::term() {
    auto expr = factor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        auto right = factor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> Parser::factor() {
    auto expr = primary();
    while (match({TokenType::STARR, TokenType::SLASH})) {
        Token op = previous();
        auto right = primary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}


std::unique_ptr<Expr> Parser::primary() {
    if (match({TokenType::NUMBER, TokenType::STAR})) return std::make_unique<LiteralExpr>(previous());
    throw std::runtime_error("Expected expression");
}
// --- تأكدي إن الجزء الأعلى من الملف موجود (constructors, peek, advance, match, ...)

std::unique_ptr<Stmt> Parser::statement() {
    // if / else -> phase / eclipse
    if (match({TokenType::PHASE})) return ifStatement();

    // for -> rotate
    if (match({TokenType::ROTATE})) return forStatement();

    // while -> orbit
    if (match({TokenType::ORBIT})) return whileStatement();

    // return -> blackHole
    if (match({TokenType::BLACKHOLE})) return returnStatement();

    // break -> darkMatter
    if (match({TokenType::DARKMATTER})) return breakStatement();

    // continue -> warp
    if (match({TokenType::WARP})) return continueStatement();

    // block
    if (match({TokenType::LEFT_BRACE})) return block();

    // expression statement (including shine(...) calls etc.)
    return expressionStatement();
}

std::unique_ptr<Stmt> Parser::ifStatement() {
    // we already consumed PHASE
    if (!match({TokenType::LEFT_PAREN})) throw std::runtime_error("Expected '(' after 'phase'");
    auto condition = expression();
    if (!match({TokenType::RIGHT_PAREN})) throw std::runtime_error("Expected ')' after condition");
    auto thenBranch = statement();

    std::unique_ptr<Stmt> elseBranch = nullptr;
    if (match({TokenType::ECLIPSE})) {
        elseBranch = statement();
    }

    auto stmt = std::make_unique<IfStmt>();
    stmt->condition = std::move(condition);
    stmt->thenBranch = std::move(thenBranch);
    stmt->elseBranch = std::move(elseBranch);
    return stmt;
}

std::unique_ptr<Stmt> Parser::whileStatement() {
    // consumed ORBIT
    if (!match({TokenType::LEFT_PAREN})) throw std::runtime_error("Expected '(' after 'orbit'");
    auto condition = expression();
    if (!match({TokenType::RIGHT_PAREN})) throw std::runtime_error("Expected ')' after condition");
    auto body = statement();

    auto stmt = std::make_unique<WhileStmt>();
    stmt->condition = std::move(condition);
    stmt->body = std::move(body);
    return stmt;
}

std::unique_ptr<Stmt> Parser::forStatement() {
    // consumed ROTATE
    if (!match({TokenType::LEFT_PAREN})) throw std::runtime_error("Expected '(' after 'rotate'");

    // initializer: could be variable declaration or expression or ';'
    std::unique_ptr<Stmt> initializer = nullptr;
    if (!match({TokenType::SEMICOLON})) {
        // try var decl
        if (match({TokenType::MASS, TokenType::FLUX, TokenType::QUANTUM, TokenType::VACUUM})) {
            // rollback one token? we consumed the type; we need to let varDeclaration handle it.
            // simpler: construct varDeclaration manually
            Token type = previous();
            Token name = advance();
            auto var = std::make_unique<VarDecl>();
            var->type = type;
            var->name = name;
            if (match({TokenType::EQUAL})) var->initializer = expression();
            if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after for initializer");
            initializer = std::move(var);
        } else {
            // expression statement as initializer
            // parse expression then require semicolon
            auto expr = expression();
            if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after for initializer");
            initializer = std::make_unique<ExprStmt>(std::move(expr));
        }
    }

    // condition
    std::unique_ptr<Expr> condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after loop condition");

    // increment
    std::unique_ptr<Expr> increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    if (!match({TokenType::RIGHT_PAREN})) throw std::runtime_error("Expected ')' after for clauses");

    // body
    auto body = statement();

    auto stmt = std::make_unique<ForStmt>();
    stmt->initializer = std::move(initializer);
    stmt->condition = std::move(condition);
    stmt->increment = std::move(increment);
    stmt->body = std::move(body);
    return stmt;
}

std::unique_ptr<Stmt> Parser::returnStatement() {
    // consumed BLACKHOLE
    std::unique_ptr<Expr> value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }
    if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after return");
    auto stmt = std::make_unique<ReturnStmt>();
    stmt->value = std::move(value);
    return stmt;
}

std::unique_ptr<Stmt> Parser::breakStatement() {
    // consumed DARKMATTER
    if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after break");
    return std::make_unique<BreakStmt>();
}

std::unique_ptr<Stmt> Parser::continueStatement() {
    // consumed WARP
    if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after continue");
    return std::make_unique<ContinueStmt>();
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
    auto expr = expression();
    if (!match({TokenType::SEMICOLON})) throw std::runtime_error("Expected ';' after expression");
    return std::make_unique<ExprStmt>(std::move(expr));
}


