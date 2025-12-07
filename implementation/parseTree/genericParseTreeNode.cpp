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

    // Constructor
    Node(NodeType nodeType, const Token& tok = Token{TokenType::ERROR, "", "", 0, 0})
        : type(nodeType), token(tok), value("") {}

    Node(NodeType nodeType, const Token& tok, const std::string& val)
        : type(nodeType), token(tok), value(val) {}

    // Destructor
    virtual ~Node() = default;

    // Add a child node
    void addChild(std::shared_ptr<Node> child) {
        if (child) {
            children.push_back(child);
        }
    }

    // Add multiple children
    void addChildren(const std::vector<std::shared_ptr<Node>>& newChildren) {
        for (const auto& child : newChildren) {
            if (child) {
                children.push_back(child);
            }
        }
    }

    // Get number of children
    size_t getChildCount() const {
        return children.size();
    }

    // Get a child by index
    std::shared_ptr<Node> getChild(size_t index) const {
        if (index < children.size()) {
            return children[index];
        }
        return nullptr;
    }

    // Helper to get node type as string (for debugging)
    std::string nodeTypeToString() const {
        switch (type) {
            case NodeType::PROGRAM: return "PROGRAM";
            case NodeType::LAUNCH_DIRECTIVE: return "LAUNCH_DIRECTIVE";
            case NodeType::GLOBAL_STATEMENT_LIST: return "GLOBAL_STATEMENT_LIST";
            case NodeType::MAIN_FUNCTION: return "MAIN_FUNCTION";
            case NodeType::DECLARATION: return "DECLARATION";
            case NodeType::DECLARATION_TAIL: return "DECLARATION_TAIL";
            case NodeType::ASSIGNMENT: return "ASSIGNMENT";
            case NodeType::FUNCTION_DECLARATION: return "FUNCTION_DECLARATION";
            case NodeType::PARAMETER_LIST: return "PARAMETER_LIST";
            case NodeType::PARAMETER: return "PARAMETER";
            case NodeType::STATEMENT_LIST: return "STATEMENT_LIST";
            case NodeType::PHASE_STATEMENT: return "PHASE_STATEMENT";
            case NodeType::ORBIT_STATEMENT: return "ORBIT_STATEMENT";
            case NodeType::ROTATE_STATEMENT: return "ROTATE_STATEMENT";
            case NodeType::SUPERNOVA_STATEMENT: return "SUPERNOVA_STATEMENT";
            case NodeType::OUTPUT_STATEMENT: return "OUTPUT_STATEMENT";
            case NodeType::INPUT_STATEMENT: return "INPUT_STATEMENT";
            case NodeType::RETURN_STATEMENT: return "RETURN_STATEMENT";
            case NodeType::BREAK_STATEMENT: return "BREAK_STATEMENT";
            case NodeType::CONTINUE_STATEMENT: return "CONTINUE_STATEMENT";
            case NodeType::STARPATH_LIST: return "STARPATH_LIST";
            case NodeType::STARPATH: return "STARPATH";
            case NodeType::BLACKVOID: return "BLACKVOID";
            case NodeType::EXPRESSION: return "EXPRESSION";
            case NodeType::TERM: return "TERM";
            case NodeType::FACTOR: return "FACTOR";
            case NodeType::CONDITION: return "CONDITION";
            case NodeType::EXPRESSION_LIST: return "EXPRESSION_LIST";
            case NodeType::TOKEN: return "TOKEN";
            default: return "UNKNOWN";
        }
    }

    // Debug: Print tree structure
    void printTree(int depth = 0) const {
        std::string indent(depth * 2, ' ');
        std::cout << indent << nodeTypeToString();
        if (!value.empty()) {
            std::cout << " [" << value << "]";
        }
        if (token.type != TokenType::ERROR) {
            std::cout << " <" << token.lexeme << ">";
        }
        std::cout << std::endl;

        for (const auto& child : children) {
            if (child) {
                child->printTree(depth + 1);
            }
        }
    }
};