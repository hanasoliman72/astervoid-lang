#include "SemanticAnalyzer.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer() {}

void SemanticAnalyzer::analyze(std::shared_ptr<Node> root) {
    if (!root) {
        std::cerr << "Semantic Error: Empty Parse Tree\n";
    }

    analyzeProgram(root);
}

std::string SemanticAnalyzer::mapLanguageTypeToInternal(const std::string& langType) {
    if (langType == "mass") return "int";
    if (langType == "flux") return "float";
    if (langType == "quantum") return "double";
    if (langType == "nebula") return "char";
    if (langType == "star") return "string";
    if (langType == "truth") return "bool";
    if (langType == "vacuum") return "void";
    return langType;
}

void SemanticAnalyzer::analyzeProgram(std::shared_ptr<Node> node) {
    if (node->type != NodeType::PROGRAM) return;

    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (!child) continue;

        if (child->type == NodeType::GLOBAL_STATEMENT_LIST) {
            analyzeGlobalStatementList(child);
        }
        else if (child->type == NodeType::MAIN_FUNCTION) {
            analyzeMainFunction(child);
        }
    }
}

void SemanticAnalyzer::analyzeGlobalStatementList(std::shared_ptr<Node> node) {
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (!child) continue;

        if (child->type == NodeType::DECLARATION) {
            analyzeDeclaration(child);
        }
        else if (child->type == NodeType::FUNCTION_DECLARATION) {
            analyzeFunctionDeclaration(child);
        }
    }
}

void SemanticAnalyzer::analyzeMainFunction(std::shared_ptr<Node> node) {
    symbolTable.pushScope();

    // Find statement list in main function
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child && child->type == NodeType::STATEMENT_LIST) {
            analyzeStatementList(child);
        }
    }

    symbolTable.popScope();
}

void SemanticAnalyzer::analyzeStatementList(std::shared_ptr<Node> node) {
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child) {
            analyzeStatement(child);
        }
    }
}

void SemanticAnalyzer::analyzeStatement(std::shared_ptr<Node> node) {
    switch (node->type) {
        case NodeType::DECLARATION:
            analyzeDeclaration(node);
            break;
        case NodeType::ASSIGNMENT:
            analyzeAssignment(node);
            break;
        case NodeType::IF_STATEMENT:
            analyzeIfStatement(node);
            break;
        case NodeType::WHILE_STATEMENT:
            analyzeWhileStatement(node);
            break;
        case NodeType::FOR_STATEMENT:
            analyzeForStatement(node);
            break;
        case NodeType::SWITCH_STATEMENT:
            analyzeSwitchStatement(node);
            break;
        case NodeType::OUTPUT_STATEMENT:
            analyzeOutputStatement(node);
            break;
        case NodeType::INPUT_STATEMENT:
            analyzeInputStatement(node);
            break;
        case NodeType::RETURN_STATEMENT:
            analyzeReturnStatement(node);
            break;
        case NodeType::BREAK_STATEMENT:
        case NodeType::CONTINUE_STATEMENT:
            break;
        default:
            break;
    }
}

void SemanticAnalyzer::analyzeDeclaration(std::shared_ptr<Node> node) {
    if (node->getChildCount() < 2) return;

    // First child: type token
    auto typeNode = node->getChild(0);
    std::string langType = typeNode->value;
    std::string internalType = mapLanguageTypeToInternal(langType);

    // Second child: identifier token
    auto idNode = node->getChild(1);
    std::string varName = idNode->value;
    int line = idNode->token.line;
    int col = idNode->token.col;

    // Declare in symbol table
    symbolTable.declare(varName, internalType, line, col);

    // If there's an initialization (DECLARATION_TAIL)
    if (node->getChildCount() > 2) {
        auto tailNode = node->getChild(2);
        if (tailNode->type == NodeType::DECLARATION_TAIL && tailNode->getChildCount() > 1) {
            // tailNode children: EQUAL token, EXPRESSION
            auto exprNode = tailNode->getChild(1);
            std::string exprType = analyzeExpression(exprNode);

            // Check type compatibility
            if (!areTypesCompatible(internalType, exprType)) {
                reportTypeError(internalType, exprType, line, col);
            }
        }
    }
}

void SemanticAnalyzer::analyzeAssignment(std::shared_ptr<Node> node) {
    if (node->getChildCount() < 3) return;

    // First child: identifier
    auto idNode = node->getChild(0);
    std::string varName = idNode->value;
    int line = idNode->token.line;
    int col = idNode->token.col;

    // Look up variable in symbol table
    Symbol* sym = symbolTable.lookup(varName, line, col);
    if (!sym) return; // Error already reported by lookup

    // Third child: expression (skip EQUAL token at index 1)
    auto exprNode = node->getChild(2);
    std::string exprType = analyzeExpression(exprNode);

    // Check type compatibility
    if (!areTypesCompatible(sym->type, exprType)) {
        reportTypeError(sym->type, exprType, line, col);
    }
}

void SemanticAnalyzer::analyzeFunctionDeclaration(std::shared_ptr<Node> node) {
    if (node->getChildCount() < 2) return;

    // Get return type and function name
    auto typeNode = node->getChild(0);
    auto nameNode = node->getChild(1);

    std::string returnType = mapLanguageTypeToInternal(typeNode->value);
    std::string funcName = nameNode->value;

    // Declare function in symbol table
    symbolTable.declare(funcName, returnType, nameNode->token.line, nameNode->token.col);

    // Push new scope for function body
    symbolTable.pushScope();

    // Process parameters
    for (size_t i = 2; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::PARAMETER_LIST) {
            for (size_t j = 0; j < child->getChildCount(); j++) {
                auto param = child->getChild(j);
                if (param->type == NodeType::PARAMETER && param->getChildCount() >= 2) {
                    std::string paramType = mapLanguageTypeToInternal(param->getChild(0)->value);
                    std::string paramName = param->getChild(1)->value;
                    symbolTable.declare(paramName, paramType, param->getChild(1)->token.line,
                                      param->getChild(1)->token.col);
                }
            }
        }
        else if (child->type == NodeType::STATEMENT_LIST) {
            analyzeStatementList(child);
        }
    }

    symbolTable.popScope();
}

void SemanticAnalyzer::analyzeIfStatement(std::shared_ptr<Node> node) {
    // Find and analyze condition
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::CONDITION) {
            std::string condType = analyzeCondition(child);
            if (condType != "bool") {
                reportError("Phase condition must be boolean type",
                          child->token.line, child->token.col);
            }
        }
        else if (child->type == NodeType::STATEMENT_LIST) {
            symbolTable.pushScope();
            analyzeStatementList(child);
            symbolTable.popScope();
        }
    }
}

void SemanticAnalyzer::analyzeWhileStatement(std::shared_ptr<Node> node) {
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::CONDITION) {
            std::string condType = analyzeCondition(child);
            if (condType != "bool") {
                reportError("Orbit condition must be boolean type",
                          child->token.line, child->token.col);
            }
        }
        else if (child->type == NodeType::STATEMENT_LIST) {
            symbolTable.pushScope();
            analyzeStatementList(child);
            symbolTable.popScope();
        }
    }
}

void SemanticAnalyzer::analyzeForStatement(std::shared_ptr<Node> node) {
    symbolTable.pushScope();

    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);

        // Analyze initialization (declaration/assignment)
        if (child->type == NodeType::DECLARATION) {
            analyzeDeclaration(child);
        }
        else if (child->type == NodeType::ASSIGNMENT) {
            analyzeAssignment(child);
        }
        // Analyze condition
        else if (child->type == NodeType::CONDITION) {
            std::string condType = analyzeCondition(child);
            if (condType != "bool") {
                reportError("Rotate condition must be boolean type",
                          child->token.line, child->token.col);
            }
        }
        // Analyze body
        else if (child->type == NodeType::STATEMENT_LIST) {
            analyzeStatementList(child);
        }
    }

    symbolTable.popScope();
}

void SemanticAnalyzer::analyzeSwitchStatement(std::shared_ptr<Node> node) {


    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::STARPATH_LIST) {
            // Analyze each case
            for (size_t j = 0; j < child->getChildCount(); j++) {
                auto caseNode = child->getChild(j);
                if (caseNode->type == NodeType::CASE) {
                    symbolTable.pushScope();
                    for (size_t k = 0; k < caseNode->getChildCount(); k++) {
                        auto caseChild = caseNode->getChild(k);
                        if (caseChild->type == NodeType::STATEMENT_LIST) {
                            analyzeStatementList(caseChild);
                        }
                    }
                    symbolTable.popScope();
                }
            }
        }
        else if (child->type == NodeType::DEFAULT) {
            // Analyze default case
            symbolTable.pushScope();
            for (size_t j = 0; j < child->getChildCount(); j++) {
                auto defaultChild = child->getChild(j);
                if (defaultChild->type == NodeType::STATEMENT_LIST) {
                    analyzeStatementList(defaultChild);
                }
            }
            symbolTable.popScope();
        }
    }
}

void SemanticAnalyzer::analyzeOutputStatement(std::shared_ptr<Node> node) {
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::EXPRESSION_LIST) {
            for (size_t j = 0; j < child->getChildCount(); j++) {
                auto expr = child->getChild(j);
                if (expr->type == NodeType::EXPRESSION) {
                    analyzeExpression(expr);
                }
            }
        }
    }
}

void SemanticAnalyzer::analyzeInputStatement(std::shared_ptr<Node> node) {
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::TOKEN && child->token.type == TokenType::IDENTIFIER) {
            symbolTable.lookup(child->value, child->token.line, child->token.col);
        }
    }
}

void SemanticAnalyzer::analyzeReturnStatement(std::shared_ptr<Node> node) {
    // If there's a return expression, analyze it
    for (size_t i = 0; i < node->getChildCount(); i++) {
        auto child = node->getChild(i);
        if (child->type == NodeType::EXPRESSION) {
            analyzeExpression(child);
        }
    }
}

std::string SemanticAnalyzer::analyzeExpression(std::shared_ptr<Node> node) {
    if (!node || node->type != NodeType::EXPRESSION) return "error";

    if (node->getChildCount() == 0) return "error";

    // Single term (no operators)
    if (node->getChildCount() == 1) {
        return analyzeTerm(node->getChild(0));
    }

    // Expression with operators: term (op term)*
    std::string currentType = analyzeTerm(node->getChild(0));

    for (size_t i = 1; i < node->getChildCount(); i += 2) {
        if (i + 1 >= node->getChildCount()) break;

        auto opNode = node->getChild(i);
        auto termNode = node->getChild(i + 1);

        std::string rightType = analyzeTerm(termNode);
        std::string op = opNode->value;

        currentType = inferBinaryOperationType(currentType, rightType, op);
    }

    nodeTypes[node] = currentType;
    return currentType;
}

std::string SemanticAnalyzer::analyzeTerm(std::shared_ptr<Node> node) {
    if (!node || node->type != NodeType::TERM) return "error";

    if (node->getChildCount() == 0) return "error";

    // Single factor
    if (node->getChildCount() == 1) {
        return analyzeFactor(node->getChild(0));
    }

    // Term with operators: factor (op factor)*
    std::string currentType = analyzeFactor(node->getChild(0));

    for (size_t i = 1; i < node->getChildCount(); i += 2) {
        if (i + 1 >= node->getChildCount()) break;

        auto opNode = node->getChild(i);
        auto factorNode = node->getChild(i + 1);

        std::string rightType = analyzeFactor(factorNode);
        std::string op = opNode->value;

        currentType = inferBinaryOperationType(currentType, rightType, op);
    }

    nodeTypes[node] = currentType;
    return currentType;
}

std::string SemanticAnalyzer::analyzeFactor(std::shared_ptr<Node> node) {
    if (!node || node->type != NodeType::FACTOR) return "error";

    // Check token type to determine literal type
    if (node->token.type == TokenType::NUMBER) {
        // Determine if int or float based on presence of decimal point
        if (node->value.find('.') != std::string::npos) {
            nodeTypes[node] = "float";
            return "float";
        } else {
            nodeTypes[node] = "int";
            return "int";
        }
    }
    else if (node->token.type == TokenType::STAR) {
        // String literal
        nodeTypes[node] = "string";
        return "string";
    }
    else if (node->token.type == TokenType::NEBULA) {
        // Character literal
        nodeTypes[node] = "char";
        return "char";
    }
    else if (node->token.type == TokenType::STARLIGHT || node->token.type == TokenType::VOIDNESS) {
        // Boolean literal
        nodeTypes[node] = "bool";
        return "bool";
    }
    else if (node->token.type == TokenType::IDENTIFIER) {
        // Variable or function call
        Symbol* sym = symbolTable.lookup(node->value, node->token.line, node->token.col);
        if (sym) {
            nodeTypes[node] = sym->type;
            return sym->type;
        }
        return "error";
    }
    else if (node->getChildCount() > 0) {
        // Parenthesized expression or function call
        auto firstChild = node->getChild(0);

        // Check if it's a parenthesized expression
        if (firstChild->type == NodeType::TOKEN && firstChild->token.type == TokenType::LEFT_PAREN) {
            // Find the expression child
            for (size_t i = 1; i < node->getChildCount(); i++) {
                if (node->getChild(i)->type == NodeType::EXPRESSION) {
                    std::string exprType = analyzeExpression(node->getChild(i));
                    nodeTypes[node] = exprType;
                    return exprType;
                }
            }
        }
        else {
            // Function call - analyze arguments
            for (size_t i = 0; i < node->getChildCount(); i++) {
                auto child = node->getChild(i);
                if (child->type == NodeType::EXPRESSION) {
                    analyzeExpression(child);
                }
            }

            // For now, assume function returns int (would need function table for proper checking)
            nodeTypes[node] = "int";
            return "int";
        }
    }

    return "error";
}

std::string SemanticAnalyzer::analyzeCondition(std::shared_ptr<Node> node) {
    if (!node || node->type != NodeType::CONDITION) return "error";

    if (node->getChildCount() < 3) return "error";

    // Analyze left expression
    auto leftExpr = node->getChild(0);
    std::string leftType = analyzeExpression(leftExpr);

    // Get operator
    auto opNode = node->getChild(1);
    std::string op = opNode->value;

    // Analyze right expression
    auto rightExpr = node->getChild(2);
    std::string rightType = analyzeExpression(rightExpr);

    // Check type compatibility
    if (!areTypesCompatible(leftType, rightType)) {
        reportError("Type mismatch in condition: " + leftType + " and " + rightType,
                   opNode->token.line, opNode->token.col);
        return "error";
    }

    // Conditions always return bool
    nodeTypes[node] = "bool";
    return "bool";
}

bool SemanticAnalyzer::areTypesCompatible(const std::string& type1, const std::string& type2) {
    if (type1 == type2) return true;
    if (type1 == "error" || type2 == "error") return true; // Don't cascade errors

    // Allow numeric conversions
    if ((type1 == "int" || type1 == "float" || type1 == "double") &&
        (type2 == "int" || type2 == "float" || type2 == "double")) {
        return true;
    }
    if ((type1 == "string" && type2 == "char") ||
            (type1 == "char" && type2 == "string")) {
        return false;
    }

    return false;
}

std::string SemanticAnalyzer::inferBinaryOperationType(const std::string& leftType,
                                                       const std::string& rightType,
                                                       const std::string& op) {
    if (leftType == "error" || rightType == "error") return "error";

    // Arithmetic operators
    if (op == "+" || op == "-" || op == "*" || op == "/" || op == "%") {
        // String concatenation
        if (op == "+" && (leftType == "string" || rightType == "string")) {
            return "string";
        }

        // Numeric operations
        if ((leftType == "int" || leftType == "float" || leftType == "double") &&
            (rightType == "int" || rightType == "float" || rightType == "double")) {

            // Result type promotion
            if (leftType == "double" || rightType == "double") return "double";
            if (leftType == "float" || rightType == "float") return "float";
            return "int";
        }

        reportError("Invalid operands for operator " + op + ": " + leftType + " and " + rightType);
        return "error";
    }

    return leftType;
}

void SemanticAnalyzer::reportError(const std::string& message, int line, int col) {
    std::cerr << "Semantic Error";
    if (line > 0) {
        std::cerr << " at line " << line;
        if (col > 0) std::cerr << ", col " << col;
    }
    std::cerr << ": " << message << std::endl;
}

void SemanticAnalyzer::reportTypeError(const std::string& expected, const std::string& actual,
                                      int line, int col) {
    reportError("Type mismatch - expected '" + expected + "' but got '" + actual + "'", line, col);
}