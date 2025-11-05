#ifndef PARSER_H
#define PARSER_H

#include "../scanner/scanner.h"
#include <vector>
#include <iostream>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    void parse(); // Entry point

private:
    const std::vector<Token>& tokens;
    int current;

    // ----------------- Grammar Rules -----------------
    void program();
    void launchDirective();
    void mainFunction();
    void statementList();
    void statement();

    void declaration();
    void declarationTail();
    void assignment();
    void phaseStmt();
    void orbitStmt();
    void rotateStmt();
    void supernovaStmt();
    void outputStmt();
    void inputStmt();
    void returnStmt();
    void darkMatterStmt();
    void warpStmt();

    void expr();
    void term();
    void factor();
    void condition();

    void paramList();
    void paramListOpt();
    void param();
    void starPathList();
    void starPath();
    void blackVoidOpt();

    void forInit();
    void conditionOpt();
    void assignmentOpt();

    // ----------------- Helper Functions -----------------
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);
    bool isAtEnd() const;
    void consume(TokenType type, const std::string& errorMessage);
};

#endif
