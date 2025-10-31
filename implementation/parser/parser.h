#ifndef PARSER_H
#define PARSER_H

#include "../Scanner/Scanner.h"
#include <memory>
#include <vector>
#include <string>

// ---------- AST Nodes ----------
struct Stmt {
    virtual ~Stmt() = default;
};

struct Expr {
    virtual ~Expr() = default;
};

struct VarDecl : Stmt {
    Token type;
    Token name;
    std::unique_ptr<Expr> initializer;
};

struct FuncDecl : Stmt {
    Token returnType;
    Token name;
    std::vector<Token> params;
    std::vector<std::unique_ptr<Stmt>> body;
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
};

struct ExprStmt : Stmt {
    std::unique_ptr<Expr> expression;
    ExprStmt(std::unique_ptr<Expr> expr) : expression(std::move(expr)) {}
};

struct IfStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> thenBranch;
    std::unique_ptr<Stmt> elseBranch; // nullable
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Stmt> body;
};

struct ForStmt : Stmt {
    // we'll keep these simple: optional init (as Stmt), condition (Expr), increment (Expr), body (Stmt)
    std::unique_ptr<Stmt> initializer; // usually VarDecl or ExprStmt
    std::unique_ptr<Expr> condition;
    std::unique_ptr<Expr> increment;
    std::unique_ptr<Stmt> body;
};

struct ReturnStmt : Stmt {
    std::unique_ptr<Expr> value; // nullable
};

struct BreakStmt : Stmt {};
struct ContinueStmt : Stmt {};

struct BinaryExpr : Expr {
    std::unique_ptr<Expr> left;
    Token op;
    std::unique_ptr<Expr> right;
    BinaryExpr(std::unique_ptr<Expr> l, Token o, std::unique_ptr<Expr> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

struct LiteralExpr : Expr {
    Token value;
    LiteralExpr(Token v) : value(v) {}
};

struct VariableExpr : Expr {
    Token name;
    VariableExpr(Token n) : name(n) {}
};

struct AssignExpr : Expr {
    Token name;
    std::unique_ptr<Expr> value;
    AssignExpr(Token n, std::unique_ptr<Expr> v) : name(n), value(std::move(v)) {}
};

// ---------- Parser Class ----------
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::vector<std::unique_ptr<Stmt>> parseProgram();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool match(std::initializer_list<TokenType> types);
    bool check(TokenType type) const;

    // Parsing
    std::unique_ptr<Stmt> declaration();
    std::unique_ptr<Stmt> function();
    std::unique_ptr<Stmt> varDeclaration();
    std::unique_ptr<Stmt> statement();
    std::unique_ptr<Stmt> block();

    // specific statements
    std::unique_ptr<Stmt> ifStatement();
    std::unique_ptr<Stmt> whileStatement();
    std::unique_ptr<Stmt> forStatement();
    std::unique_ptr<Stmt> returnStatement();
    std::unique_ptr<Stmt> breakStatement();
    std::unique_ptr<Stmt> continueStatement();
    std::unique_ptr<Stmt> expressionStatement();

    // Expressions
    std::unique_ptr<Expr> expression();
    std::unique_ptr<Expr> term();
    std::unique_ptr<Expr> factor();
    std::unique_ptr<Expr> primary();
};

#endif
