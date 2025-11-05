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
    void statementList(); // Parses a list of statements inside { ... }
    void statement(); // Parses a single statement.

    void declaration();
    void declarationTail();
    void assignment();
    void phaseStmt();
    void orbitStmt();
    void rotateStmt();
    void supernovaStmt();
    void outputStmt();
    void inputStmt();
    void darkMatterStmt();
    void warpStmt();

    void expr(); // Parses addition/subtraction.
    void term(); // Parses multiplication/division/modulus.
    void factor(); // Parses numbers, variables, parenthesis, literals.
    void condition(); // Parses relational expressions (==, !=, <, etc.).

    void starPathList();
    void starPath();
    void blackVoidOpt();

    void forInit();
    void conditionOpt();
    void assignmentOpt();

    // ----------------- Helper Functions -----------------
    const Token& peek() const; // Returns the current token
    const Token& previous() const; // Returns the previous token
    const Token& advance(); // Moves to the next token and returns the previous
    bool check(TokenType type) const; // Returns true if the current token matches type (without advancing)
    bool match(TokenType type); // Checks and consumes the token if it matches
    bool isAtEnd() const; // Checks and consumes the token if it matches
    void consume(TokenType type, const std::string& errorMessage); // Ensures the current token is of type type, advances, else throws an error
};

#endif
