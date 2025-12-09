#include "GenericParseTreeNode.h"

// ================== Constructors ==================
Node::Node(NodeType nodeType, const Token& tok)
    : type(nodeType), token(tok), value("") {}

Node::Node(NodeType nodeType, const Token& tok, const std::string& val)
    : type(nodeType), token(tok), value(val) {}

// ================== Methods ==================
void Node::addChild(std::shared_ptr<Node> child) {
    if (child) {
        children.push_back(child);
    }
}

void Node::addChildren(const std::vector<std::shared_ptr<Node>>& newChildren) {
    for (const auto& child : newChildren) {
        if (child) {
            children.push_back(child);
        }
    }
}

size_t Node::getChildCount() const {
    return children.size();
}

std::shared_ptr<Node> Node::getChild(size_t index) const {
    if (index < children.size()) {
        return children[index];
    }
    return nullptr;
}

std::string Node::nodeTypeToString() const {
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
        case NodeType::IF_STATEMENT: return "IF_STATEMENT";
        case NodeType::WHILE_STATEMENT: return "WHILE_STATEMENT";
        case NodeType::FOR_STATEMENT: return "FOR_STATEMENT";
        case NodeType::SWITCH_STATEMENT: return "SWITCH_STATEMENT";
        case NodeType::OUTPUT_STATEMENT: return "OUTPUT_STATEMENT";
        case NodeType::INPUT_STATEMENT: return "INPUT_STATEMENT";
        case NodeType::RETURN_STATEMENT: return "RETURN_STATEMENT";
        case NodeType::BREAK_STATEMENT: return "BREAK_STATEMENT";
        case NodeType::CONTINUE_STATEMENT: return "CONTINUE_STATEMENT";
        case NodeType::STARPATH_LIST: return "STARPATH_LIST";
        case NodeType::CASE: return "CASE";
        case NodeType::DEFAULT: return "DEFAULT";
        case NodeType::EXPRESSION: return "EXPRESSION";
        case NodeType::TERM: return "TERM";
        case NodeType::FACTOR: return "FACTOR";
        case NodeType::CONDITION: return "CONDITION";
        case NodeType::EXPRESSION_LIST: return "EXPRESSION_LIST";
        case NodeType::TOKEN: return "TOKEN";
        default: return "UNKNOWN";
    }
}

void Node::printTree(int depth) const {
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