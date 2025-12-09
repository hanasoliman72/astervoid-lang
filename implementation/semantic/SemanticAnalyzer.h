#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H

#include "..\parseTree\GenericParseTreeNode.h"
#include "SymbolTable.h"
#include <unordered_map>
#include <memory>
#include <string>

class SemanticAnalyzer {
public:
    SemanticAnalyzer();

    // Entry point
    void analyze(std::shared_ptr<Node> root);

private:
    SymbolTable symbolTable;
    bool hasErrors;
    std::unordered_map<std::shared_ptr<Node>, std::string> nodeTypes;

    // Type mapping from language keywords to internal types
    std::string mapLanguageTypeToInternal(const std::string& langType);

    // --------- Main traversal methods ---------
    void analyzeProgram(std::shared_ptr<Node> node);
    void analyzeGlobalStatementList(std::shared_ptr<Node> node);
    void analyzeMainFunction(std::shared_ptr<Node> node);
    void analyzeStatementList(std::shared_ptr<Node> node);
    void analyzeStatement(std::shared_ptr<Node> node);

    // --------- Grammar Rules ---------
    void analyzeDeclaration(std::shared_ptr<Node> node);
    void analyzeAssignment(std::shared_ptr<Node> node);
    void analyzeFunctionDeclaration(std::shared_ptr<Node> node);
    void analyzeIfStatement(std::shared_ptr<Node> node);
    void analyzeWhileStatement(std::shared_ptr<Node> node);
    void analyzeForStatement(std::shared_ptr<Node> node);
    void analyzeSwitchStatement(std::shared_ptr<Node> node);
    void analyzeOutputStatement(std::shared_ptr<Node> node);
    void analyzeInputStatement(std::shared_ptr<Node> node);
    void analyzeReturnStatement(std::shared_ptr<Node> node);

    // --------- Expression type checking ---------
    std::string analyzeExpression(std::shared_ptr<Node> node);
    std::string analyzeTerm(std::shared_ptr<Node> node);
    std::string analyzeFactor(std::shared_ptr<Node> node);
    std::string analyzeCondition(std::shared_ptr<Node> node);

    // --------- Type checking helpers ---------
    bool areTypesCompatible(const std::string& type1, const std::string& type2);
    std::string inferBinaryOperationType(const std::string& leftType,
                                         const std::string& rightType,
                                         const std::string& op);

    // --------- Error reporting ---------
    void reportError(const std::string& message, int line = 0, int col = 0);
    void reportTypeError(const std::string& expected, const std::string& actual,
                        int line = 0, int col = 0);
};

#endif