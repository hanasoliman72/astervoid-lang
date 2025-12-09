#include "SymbolTable.h"

// ================== Symbol Implementation ==================
Symbol::Symbol() : name(""), type(""), line(0), col(0), scope(0) {}

Symbol::Symbol(const std::string& n, const std::string& t, int l, int c, int s)
    : name(n), type(t), line(l), col(c), scope(s) {}

// ================== SymbolTable Implementation ==================
SymbolTable::SymbolTable() : currentScope(0) {
    // Initialize with global scope
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::pushScope() {
    currentScope++;
    scopes.push_back(std::unordered_map<std::string, Symbol>());
}

void SymbolTable::popScope() {
    if (scopes.size() > 1) {  // Never remove global scope
        scopes.pop_back();
        currentScope--;
    }
}

int SymbolTable::getCurrentScope() const {
    return currentScope;
}

void SymbolTable::declare(const std::string& name, const std::string& type, int line, int col) {
    auto& currentScopeMap = scopes.back();

    // Check if already declared in current scope
    if (currentScopeMap.find(name) != currentScopeMap.end()) {
        std::cerr << "Semantic Error: '" << name << "' redeclared at line " << line << std::endl;
        return;
    }

    currentScopeMap[name] = Symbol(name, type, line, col, currentScope);
}

Symbol* SymbolTable::lookup(const std::string& name, int line, int col) {
    // Search from innermost (current) to outermost (global) scope
    for (int i = (int)scopes.size() - 1; i >= 0; i--) {
        if (scopes[i].find(name) != scopes[i].end()) {
            return &scopes[i].find(name)->second;
        }
    }

    // Not found
    std::cerr << "Semantic Error: Undefined variable '" << name << "' at line " << line << std::endl;
    return nullptr;
}

bool SymbolTable::existsInCurrentScope(const std::string& name) {
    auto& currentScopeMap = scopes.back();
    return currentScopeMap.find(name) != currentScopeMap.end();
}

int SymbolTable::getScopeDepth() const {
    return (int)scopes.size();
}