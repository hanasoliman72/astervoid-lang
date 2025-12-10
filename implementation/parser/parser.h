#ifndef PARSER_H
#define PARSER_H

#include "../parseTree/genericParseTreeNode.h"
#include "../scanner/scanner.h"
#include <memory>
#include <vector>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<Node> parse();

private:
    const std::vector<Token>& tokens;
    int current;

    // --------- Grammar Rules ---------
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
    std::shared_ptr<Node> ifStmt();
    std::shared_ptr<Node> whileStmt();
    std::shared_ptr<Node> forStmt();
    std::shared_ptr<Node> switchStmt();
    std::shared_ptr<Node> outputStmt();
    std::shared_ptr<Node> inputStmt();
    std::shared_ptr<Node> breakStmt();
    std::shared_ptr<Node> continueStmt();
    std::shared_ptr<Node> returnStmt();
    std::shared_ptr<Node> functionStmt();

    std::shared_ptr<Node> expr();
    std::shared_ptr<Node> term();
    std::shared_ptr<Node> factor();
    std::shared_ptr<Node> condition();
    std::shared_ptr<Node> exprList();
    std::shared_ptr<Node> exprOpt();

    std::shared_ptr<Node> starPathList();
    std::shared_ptr<Node> casePath();
    std::shared_ptr<Node> defaultOpt();

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

#endif