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
    PHASE_STATEMENT,
    ORBIT_STATEMENT,
    ROTATE_STATEMENT,
    SUPERNOVA_STATEMENT,
    OUTPUT_STATEMENT,
    INPUT_STATEMENT,
    RETURN_STATEMENT,
    BREAK_STATEMENT,
    CONTINUE_STATEMENT,

    // Switch-like
    STARPATH_LIST,
    STARPATH,
    BLACKVOID,

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
    NodeType type;
    Token token;  // The token this node represents (for semantic analysis)
    std::vector<std::shared_ptr<Node>> children;  // Child nodes
    std::string value;  // For storing literal values, type names, identifiers, etc.

    // Constructors
    Node(NodeType nodeType, const Token& tok = Token{TokenType::ERROR, "", "", 0, 0});
    Node(NodeType nodeType, const Token& tok, const std::string& val);

    // Destructor
    virtual ~Node() = default;

    // Add a child node
    void addChild(std::shared_ptr<Node> child);

    // Add multiple children
    void addChildren(const std::vector<std::shared_ptr<Node>>& newChildren);

    // Get number of children
    size_t getChildCount() const;

    // Get a child by index
    std::shared_ptr<Node> getChild(size_t index) const;

    // Helper to get node type as string (for debugging)
    std::string nodeTypeToString() const;

    // Debug: Print tree structure
    void printTree(int depth = 0) const;
};

#endif // GENERICPARSETREENODE_H