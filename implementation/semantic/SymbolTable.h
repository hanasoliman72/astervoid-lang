#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

// ================== Symbol Entry ==================
struct Symbol {
    std::string name;
    std::string type;
    int line;
    int col;
    int scope;

    Symbol();
    Symbol(const std::string& n, const std::string& t, int l, int c, int s);
};

// ================== Symbol Table ==================
class SymbolTable {
private:
    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    int currentScope;

public:
    SymbolTable();

    // Push a new scope
    void pushScope();

    // Pop current scope
    void popScope();

    // Get current scope level
    int getCurrentScope() const;

    // Declare a symbol in current scope
    void declare(const std::string& name, const std::string& type, int line, int col);

    // Look up a symbol in all scopes (from current to global)
    Symbol* lookup(const std::string& name, int line, int col);

    // Check if symbol exists in current scope only
    bool existsInCurrentScope(const std::string& name);

    // Get scope depth
    int getScopeDepth() const;
};

#endif