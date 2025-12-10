#ifndef GENERICPARSETREENODE_H
#define GENERICPARSETREENODE_H

#include "../scanner/scanner.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

// ================== Node Type Enum ==================
enum class NodeType {
    // Program & Structure
    PROGRAM,
    LAUNCH_DIRECTIVE,
    GLOBAL_STATEMENT_LIST,
    MAIN_FUNCTION,

    // Declarations & Assignments
    DECLARATION,
    DECLARATION_TAIL,
    ASSIGNMENT,
    FUNCTION_DECLARATION,
    PARAMETER_LIST,
    PARAMETER,

    // Statements
    STATEMENT_LIST,
    IF_STATEMENT,
    WHILE_STATEMENT,
    FOR_STATEMENT,
    SWITCH_STATEMENT,
    OUTPUT_STATEMENT,
    INPUT_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,

    // Switch-like
    STARPATH_LIST,
    CASE,
    DEFAULT,

    // Expressions
    EXPRESSION,
    TERM,
    FACTOR,
    CONDITION,
    EXPRESSION_LIST,

    // Tokens (leaf nodes)
    TOKEN
};

// ================== Generic ASTNode ==================
class Node {
public:
    NodeType type; // Stores which type of node this is
    Token token;  // The token this node represents (for semantic analysis)
    // std::shared_ptr (smart pointers) automatically manages memory
    // when no references exist, the node is deleted.
    std::vector<std::shared_ptr<Node>> children;
    std::string value;  // For storing literal values, type names, identifiers.

    Node(NodeType nodeType, const Token& tok = Token{TokenType::ERROR, "", "", 0, 0});
    Node(NodeType nodeType, const Token& tok, const std::string& val);

    virtual ~Node() = default;

    // Add a child node
    void addChild(std::shared_ptr<Node> child);

    // Add multiple children
    void addChildren(const std::vector<std::shared_ptr<Node>>& newChildren);

    // Get number of children
    size_t getChildCount() const;

    // Get a child by index
    std::shared_ptr<Node> getChild(size_t index) const;

    // Helper to get node type as string
    std::string nodeTypeToString() const;

    // Print tree structure
    void printTree(int depth = 0) const;
};

#endif