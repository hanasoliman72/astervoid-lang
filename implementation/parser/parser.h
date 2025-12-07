#ifndef PARSER_H
#define PARSER_H

#include "../scanner/scanner.h"
#include "../parseTree/genericParseTreeNode.h"
#include <memory>
#include <vector>
#include <iostream>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<Node> parse(); // Entry point - returns AST root

private:
    const std::vector<Token>& tokens;
    int current;

    // --------- Grammar Rules (all return AST nodes) ---------
    std::shared_ptr<Node> program();
    std::shared_ptr<Node> launchDirective();
    std::shared_ptr<Node> globalStatementList();
    std::shared_ptr<Node> globalStatement();
    std::shared_ptr<Node> mainFunction();
    std::shared_ptr<Node> statementList();
    std::shared_ptr<Node> statement();

    std::shared_ptr<Node> declaration();
    std::shared_ptr<Node> declarationTail();
    std::shared_ptr<Node> assignment();
    std::shared_ptr<Node> phaseStmt();
    std::shared_ptr<Node> orbitStmt();
    std::shared_ptr<Node> rotateStmt();
    std::shared_ptr<Node> supernovaStmt();
    std::shared_ptr<Node> outputStmt();
    std::shared_ptr<Node> inputStmt();
    std::shared_ptr<Node> darkMatterStmt();
    std::shared_ptr<Node> warpStmt();
    std::shared_ptr<Node> returnStmt();
    std::shared_ptr<Node> functionStmt();

    std::shared_ptr<Node> expr();
    std::shared_ptr<Node> term();
    std::shared_ptr<Node> factor();
    std::shared_ptr<Node> condition();
    std::shared_ptr<Node> exprList();
    std::shared_ptr<Node> exprOpt();

    std::shared_ptr<Node> starPathList();
    std::shared_ptr<Node> starPath();
    std::shared_ptr<Node> blackVoidOpt();

    std::shared_ptr<Node> forInit();
    std::shared_ptr<Node> conditionOpt();
    std::shared_ptr<Node> assignmentOpt();

    std::shared_ptr<Node> paramList();
    std::shared_ptr<Node> param();

    // --------- Helper Functions ---------
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool isAtEnd() const;
    void consume(TokenType type, const std::string& errorMessage);
    bool isFunctionDeclaration();
};

#endif // PARSER_H