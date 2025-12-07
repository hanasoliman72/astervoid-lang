#include "Parser.h"

// -------------------- Constructor --------------------
Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0) {}

// -------------------- Entry Point --------------------
std::shared_ptr<Node> Parser::parse() {
    try {
        auto ast = program();
        std::cout << "Syntax OK!" << std::endl;
        return ast;
    } catch (const std::runtime_error& e) {
        std::cerr << "Syntax Error: " << e.what() << std::endl;
        return nullptr;
    }
}

// -------------------- Grammar Rules --------------------
std::shared_ptr<Node> Parser::program() {
    auto programNode = std::make_shared<Node>(NodeType::PROGRAM);

    auto launchDir = launchDirective();
    if (launchDir) programNode->addChild(launchDir);

    auto globalStmts = globalStatementList();
    if (globalStmts) programNode->addChild(globalStmts);

    auto mainFunc = mainFunction();
    if (mainFunc) programNode->addChild(mainFunc);

    if (!isAtEnd()) {
        throw std::runtime_error("Syntax Error: Expected end of file at line " + std::to_string(peek().line));
    }

    return programNode;
}

std::shared_ptr<Node> Parser::launchDirective() {
    Token launchToken = peek();
    consume(TokenType::LAUNCH, "Expected 'launch' directive at start");

    auto node = std::make_shared<Node>(NodeType::LAUNCH_DIRECTIVE, launchToken);
    return node;
}

std::shared_ptr<Node> Parser::globalStatementList() {
    auto listNode = std::make_shared<Node>(NodeType::GLOBAL_STATEMENT_LIST);

    while (!isAtEnd() && !check(TokenType::VACUUM) && !check(TokenType::END_OF_FILE)) {
        auto stmt = globalStatement();
        if (stmt) listNode->addChild(stmt);
    }

    return listNode;
}

std::shared_ptr<Node> Parser::globalStatement() {
    if (check(TokenType::MASS) || check(TokenType::FLUX) || check(TokenType::QUANTUM) ||
        check(TokenType::NEBULA) || check(TokenType::STAR) || check(TokenType::TRUTH) || check(TokenType::VACUUM)) {
        if (isFunctionDeclaration()) {
            return functionStmt();
        } else {
            return declaration();
        }
    }
    else {
        throw std::runtime_error("Expected type or function declaration at line " + std::to_string(peek().line));
    }
}

std::shared_ptr<Node> Parser::mainFunction() {
    auto mainNode = std::make_shared<Node>(NodeType::MAIN_FUNCTION);

    Token vacToken = peek();
    consume(TokenType::VACUUM, "Expected 'vacuum' for entry function");
    mainNode->addChild(std::make_shared<Node>(NodeType::TOKEN, vacToken));

    Token milkToken = peek();
    consume(TokenType::MILKYWAY, "Expected 'milkyway' as entry function name");
    mainNode->addChild(std::make_shared<Node>(NodeType::TOKEN, milkToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after milkyway");
    mainNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after milkyway");
    mainNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token lbToken = peek();
    consume(TokenType::LEFT_BRACE, "Expected '{' at start of main function body");
    mainNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lbToken));

    auto stmtList = statementList();
    if (stmtList) mainNode->addChild(stmtList);

    Token rbToken = peek();
    consume(TokenType::RIGHT_BRACE, "Expected '}' at end of main function body");
    mainNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rbToken));

    return mainNode;
}

std::shared_ptr<Node> Parser::statementList() {
    auto listNode = std::make_shared<Node>(NodeType::STATEMENT_LIST);

    while (!isAtEnd() && !check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        auto stmt = statement();
        if (stmt) listNode->addChild(stmt);
    }

    return listNode;
}

std::shared_ptr<Node> Parser::statement() {
    if (check(TokenType::MASS) || check(TokenType::FLUX) || check(TokenType::QUANTUM) ||
        check(TokenType::NEBULA) || check(TokenType::STAR) || check(TokenType::TRUTH) || check(TokenType::VACUUM)) {
        return declaration();
    }
    else if (check(TokenType::IDENTIFIER)) {
        return assignment();
    }
    else if (check(TokenType::PHASE)) return phaseStmt();
    else if (check(TokenType::ORBIT)) return orbitStmt();
    else if (check(TokenType::ROTATE)) return rotateStmt();
    else if (check(TokenType::SUPERNOVA)) return supernovaStmt();
    else if (check(TokenType::SHINE)) return outputStmt();
    else if (check(TokenType::MOON)) return inputStmt();
    else if (check(TokenType::BLACKHOLE)) return returnStmt();
    else if (check(TokenType::DARKMATTER)) return darkMatterStmt();
    else if (check(TokenType::WARP)) return warpStmt();
    else {
        throw std::runtime_error("Unexpected token: " + peek().lexeme);
    }
}

// -------------------- Statements --------------------
std::shared_ptr<Node> Parser::declaration() {
    auto declNode = std::make_shared<Node>(NodeType::DECLARATION);

    Token typeToken = peek();
    advance();
    declNode->addChild(std::make_shared<Node>(NodeType::TOKEN, typeToken, typeToken.lexeme));

    Token idToken = peek();
    consume(TokenType::IDENTIFIER, "Expected identifier after type");
    declNode->addChild(std::make_shared<Node>(NodeType::TOKEN, idToken, idToken.lexeme));

    auto tailNode = declarationTail();
    if (tailNode) declNode->addChild(tailNode);

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after declaration");
    declNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return declNode;
}

std::shared_ptr<Node> Parser::declarationTail() {
    auto tailNode = std::make_shared<Node>(NodeType::DECLARATION_TAIL);

    if (match(TokenType::EQUAL)) {
        tailNode->addChild(std::make_shared<Node>(NodeType::TOKEN, previous(), "="));
        auto exprNode = expr();
        if (exprNode) tailNode->addChild(exprNode);
    }

    return tailNode;
}

std::shared_ptr<Node> Parser::assignment() {
    auto assignNode = std::make_shared<Node>(NodeType::ASSIGNMENT);

    Token idToken = peek();
    advance();
    assignNode->addChild(std::make_shared<Node>(NodeType::TOKEN, idToken, idToken.lexeme));

    Token eqToken = peek();
    consume(TokenType::EQUAL, "Expected '=' in assignment");
    assignNode->addChild(std::make_shared<Node>(NodeType::TOKEN, eqToken));

    auto exprNode = expr();
    if (exprNode) assignNode->addChild(exprNode);

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    assignNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return assignNode;
}

std::shared_ptr<Node> Parser::phaseStmt() {
    auto phaseNode = std::make_shared<Node>(NodeType::PHASE_STATEMENT);

    Token phaseToken = peek();
    consume(TokenType::PHASE, "Expected 'phase'");
    phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, phaseToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'phase'");
    phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    auto condNode = condition();
    if (condNode) phaseNode->addChild(condNode);

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token lbToken = peek();
    consume(TokenType::LEFT_BRACE, "Expected '{' for phase block");
    phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lbToken));

    auto stmtListNode = statementList();
    if (stmtListNode) phaseNode->addChild(stmtListNode);

    Token rbToken = peek();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after phase block");
    phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rbToken));

    if (match(TokenType::ECLIPSE)) {
        Token eclipseToken = previous();
        phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, eclipseToken));

        Token elseB = peek();
        consume(TokenType::LEFT_BRACE, "Expected '{' for eclipse block");
        phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, elseB));

        auto elseStmtList = statementList();
        if (elseStmtList) phaseNode->addChild(elseStmtList);

        Token elseRb = peek();
        consume(TokenType::RIGHT_BRACE, "Expected '}' after eclipse block");
        phaseNode->addChild(std::make_shared<Node>(NodeType::TOKEN, elseRb));
    }

    return phaseNode;
}

std::shared_ptr<Node> Parser::orbitStmt() {
    auto orbitNode = std::make_shared<Node>(NodeType::ORBIT_STATEMENT);

    Token orbitToken = peek();
    consume(TokenType::ORBIT, "Expected 'orbit'");
    orbitNode->addChild(std::make_shared<Node>(NodeType::TOKEN, orbitToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'orbit'");
    orbitNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    auto condNode = condition();
    if (condNode) orbitNode->addChild(condNode);

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition");
    orbitNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token lbToken = peek();
    consume(TokenType::LEFT_BRACE, "Expected '{' for orbit block");
    orbitNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lbToken));

    auto stmtListNode = statementList();
    if (stmtListNode) orbitNode->addChild(stmtListNode);

    Token rbToken = peek();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after orbit block");
    orbitNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rbToken));

    return orbitNode;
}

std::shared_ptr<Node> Parser::rotateStmt() {
    auto rotateNode = std::make_shared<Node>(NodeType::ROTATE_STATEMENT);

    Token rotateToken = peek();
    consume(TokenType::ROTATE, "Expected 'rotate'");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rotateToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'rotate'");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    auto forInitNode = forInit();
    if (forInitNode) rotateNode->addChild(forInitNode);

    Token semi1 = peek();
    consume(TokenType::SEMICOLON, "Expected ';' in rotate");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semi1));

    auto condOptNode = conditionOpt();
    if (condOptNode) rotateNode->addChild(condOptNode);

    Token semi2 = peek();
    consume(TokenType::SEMICOLON, "Expected ';' in rotate");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semi2));

    auto assignOptNode = assignmentOpt();
    if (assignOptNode) rotateNode->addChild(assignOptNode);

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after rotate header");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token lbToken = peek();
    consume(TokenType::LEFT_BRACE, "Expected '{' for rotate block");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lbToken));

    auto stmtListNode = statementList();
    if (stmtListNode) rotateNode->addChild(stmtListNode);

    Token rbToken = peek();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after rotate block");
    rotateNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rbToken));

    return rotateNode;
}

std::shared_ptr<Node> Parser::forInit() {
    auto initNode = std::make_shared<Node>(NodeType::DECLARATION);

    if (check(TokenType::MASS) || check(TokenType::FLUX) || check(TokenType::QUANTUM) ||
        check(TokenType::NEBULA) || check(TokenType::STAR) || check(TokenType::TRUTH) || check(TokenType::VACUUM)) {
        Token typeToken = peek();
        advance();
        initNode->addChild(std::make_shared<Node>(NodeType::TOKEN, typeToken, typeToken.lexeme));

        Token idToken = peek();
        consume(TokenType::IDENTIFIER, "Expected identifier in for-init");
        initNode->addChild(std::make_shared<Node>(NodeType::TOKEN, idToken, idToken.lexeme));

        if (match(TokenType::EQUAL)) {
            initNode->addChild(std::make_shared<Node>(NodeType::TOKEN, previous(), "="));
            auto exprNode = expr();
            if (exprNode) initNode->addChild(exprNode);
        }
    }
    else if (check(TokenType::IDENTIFIER)) {
        Token idToken = peek();
        advance();
        initNode->addChild(std::make_shared<Node>(NodeType::TOKEN, idToken, idToken.lexeme));

        Token eqToken = peek();
        consume(TokenType::EQUAL, "Expected '=' in assignment for for-init");
        initNode->addChild(std::make_shared<Node>(NodeType::TOKEN, eqToken));

        auto exprNode = expr();
        if (exprNode) initNode->addChild(exprNode);
    }

    return initNode;
}

std::shared_ptr<Node> Parser::conditionOpt() {
    if (!check(TokenType::SEMICOLON)) {
        return condition();
    }
    return nullptr;
}

std::shared_ptr<Node> Parser::assignmentOpt() {
    if (!check(TokenType::RIGHT_PAREN)) {
        Token idToken = peek();
        advance();

        auto assignNode = std::make_shared<Node>(NodeType::ASSIGNMENT);
        assignNode->addChild(std::make_shared<Node>(NodeType::TOKEN, idToken, idToken.lexeme));

        Token eqToken = peek();
        consume(TokenType::EQUAL, "Expected '=' in assignment");
        assignNode->addChild(std::make_shared<Node>(NodeType::TOKEN, eqToken));

        auto exprNode = expr();
        if (exprNode) assignNode->addChild(exprNode);

        return assignNode;
    }
    return nullptr;
}

std::shared_ptr<Node> Parser::supernovaStmt() {
    auto superNode = std::make_shared<Node>(NodeType::SUPERNOVA_STATEMENT);

    Token superToken = peek();
    consume(TokenType::SUPERNOVA, "Expected 'supernova'");
    superNode->addChild(std::make_shared<Node>(NodeType::TOKEN, superToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'supernova'");
    superNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    auto exprNode = expr();
    if (exprNode) superNode->addChild(exprNode);

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
    superNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token lbToken = peek();
    consume(TokenType::LEFT_BRACE, "Expected '{' for supernova block");
    superNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lbToken));

    auto pathListNode = starPathList();
    if (pathListNode) superNode->addChild(pathListNode);

    auto blackVoidNode = blackVoidOpt();
    if (blackVoidNode) superNode->addChild(blackVoidNode);

    Token rbToken = peek();
    consume(TokenType::RIGHT_BRACE, "Expected '}' after supernova block");
    superNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rbToken));

    return superNode;
}

std::shared_ptr<Node> Parser::starPathList() {
    auto listNode = std::make_shared<Node>(NodeType::STARPATH_LIST);

    while (check(TokenType::STARPATH)) {
        auto pathNode = starPath();
        if (pathNode) listNode->addChild(pathNode);
    }

    return listNode;
}

std::shared_ptr<Node> Parser::starPath() {
    auto pathNode = std::make_shared<Node>(NodeType::STARPATH);

    Token starToken = peek();
    consume(TokenType::STARPATH, "Expected 'starPath'");
    pathNode->addChild(std::make_shared<Node>(NodeType::TOKEN, starToken));

    Token numToken = peek();
    consume(TokenType::NUMBER, "Expected number after 'starPath'");
    pathNode->addChild(std::make_shared<Node>(NodeType::TOKEN, numToken, numToken.literal));

    Token colToken = peek();
    consume(TokenType::COLON, "Expected ':' after starPath number");
    pathNode->addChild(std::make_shared<Node>(NodeType::TOKEN, colToken));

    auto stmtListNode = statementList();
    if (stmtListNode) pathNode->addChild(stmtListNode);

    return pathNode;
}

std::shared_ptr<Node> Parser::blackVoidOpt() {
    if (match(TokenType::BLACKVOID)) {
        auto blackVoidNode = std::make_shared<Node>(NodeType::BLACKVOID);
        blackVoidNode->addChild(std::make_shared<Node>(NodeType::TOKEN, previous()));

        Token colToken = peek();
        consume(TokenType::COLON, "Expected ':' after blackVoid");
        blackVoidNode->addChild(std::make_shared<Node>(NodeType::TOKEN, colToken));

        auto stmtListNode = statementList();
        if (stmtListNode) blackVoidNode->addChild(stmtListNode);

        return blackVoidNode;
    }
    return nullptr;
}

std::shared_ptr<Node> Parser::outputStmt() {
    auto shineNode = std::make_shared<Node>(NodeType::OUTPUT_STATEMENT);

    Token shineToken = peek();
    consume(TokenType::SHINE, "Expected 'shine'");
    shineNode->addChild(std::make_shared<Node>(NodeType::TOKEN, shineToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after shine");
    shineNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    auto exprListNode = exprList();
    if (exprListNode) shineNode->addChild(exprListNode);

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after shine");
    shineNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after shine statement");
    shineNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return shineNode;
}

std::shared_ptr<Node> Parser::inputStmt() {
    auto moonNode = std::make_shared<Node>(NodeType::INPUT_STATEMENT);

    Token moonToken = peek();
    consume(TokenType::MOON, "Expected 'moon'");
    moonNode->addChild(std::make_shared<Node>(NodeType::TOKEN, moonToken));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after moon");
    moonNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    Token idToken = peek();
    consume(TokenType::IDENTIFIER, "Expected variable name in moon");
    moonNode->addChild(std::make_shared<Node>(NodeType::TOKEN, idToken, idToken.lexeme));

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after moon");
    moonNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after moon statement");
    moonNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return moonNode;
}

std::shared_ptr<Node> Parser::darkMatterStmt() {
    auto darkNode = std::make_shared<Node>(NodeType::BREAK_STATEMENT);

    Token darkToken = peek();
    consume(TokenType::DARKMATTER, "Expected 'darkMatter'");
    darkNode->addChild(std::make_shared<Node>(NodeType::TOKEN, darkToken));

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after darkMatter");
    darkNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return darkNode;
}

std::shared_ptr<Node> Parser::warpStmt() {
    auto warpNode = std::make_shared<Node>(NodeType::CONTINUE_STATEMENT);

    Token warpToken = peek();
    consume(TokenType::WARP, "Expected 'warp'");
    warpNode->addChild(std::make_shared<Node>(NodeType::TOKEN, warpToken));

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after warp");
    warpNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return warpNode;
}

std::shared_ptr<Node> Parser::returnStmt() {
    auto retNode = std::make_shared<Node>(NodeType::RETURN_STATEMENT);

    Token retToken = peek();
    consume(TokenType::BLACKHOLE, "Expected 'blackHole'");
    retNode->addChild(std::make_shared<Node>(NodeType::TOKEN, retToken));

    auto exprOptNode = exprOpt();
    if (exprOptNode) retNode->addChild(exprOptNode);

    Token semiToken = peek();
    consume(TokenType::SEMICOLON, "Expected ';' after return statement");
    retNode->addChild(std::make_shared<Node>(NodeType::TOKEN, semiToken));

    return retNode;
}

std::shared_ptr<Node> Parser::functionStmt() {
    auto funcNode = std::make_shared<Node>(NodeType::FUNCTION_DECLARATION);

    Token typeToken = peek();
    advance();
    funcNode->addChild(std::make_shared<Node>(NodeType::TOKEN, typeToken, typeToken.lexeme));

    Token nameToken = peek();
    consume(TokenType::IDENTIFIER, "Expected function name");
    funcNode->addChild(std::make_shared<Node>(NodeType::TOKEN, nameToken, nameToken.lexeme));

    Token lpToken = peek();
    consume(TokenType::LEFT_PAREN, "Expected '(' after function name");
    funcNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));

    auto paramListNode = paramList();
    if (paramListNode) funcNode->addChild(paramListNode);

    Token rpToken = peek();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameter list");
    funcNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));

    Token lbToken = peek();
    consume(TokenType::LEFT_BRACE, "Expected '{' at start of function body");
    funcNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lbToken));

    auto stmtListNode = statementList();
    if (stmtListNode) funcNode->addChild(stmtListNode);

    Token rbToken = peek();
    consume(TokenType::RIGHT_BRACE, "Expected '}' at end of function body");
    funcNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rbToken));

    return funcNode;
}

std::shared_ptr<Node> Parser::paramList() {
    auto listNode = std::make_shared<Node>(NodeType::PARAMETER_LIST);

    if (!check(TokenType::RIGHT_PAREN)) {
        auto paramNode = param();
        if (paramNode) listNode->addChild(paramNode);

        while (match(TokenType::COMMA)) {
            listNode->addChild(std::make_shared<Node>(NodeType::TOKEN, previous(), ","));
            auto nextParam = param();
            if (nextParam) listNode->addChild(nextParam);
        }
    }

    return listNode;
}

std::shared_ptr<Node> Parser::param() {
    auto paramNode = std::make_shared<Node>(NodeType::PARAMETER);

    if (check(TokenType::MASS) || check(TokenType::FLUX) || check(TokenType::QUANTUM) ||
        check(TokenType::NEBULA) || check(TokenType::STAR) || check(TokenType::TRUTH) || check(TokenType::VACUUM)) {
        Token typeToken = peek();
        advance();
        paramNode->addChild(std::make_shared<Node>(NodeType::TOKEN, typeToken, typeToken.lexeme));

        Token nameToken = peek();
        consume(TokenType::IDENTIFIER, "Expected parameter name");
        paramNode->addChild(std::make_shared<Node>(NodeType::TOKEN, nameToken, nameToken.lexeme));

        return paramNode;
    } else {
        throw std::runtime_error("Expected type in parameter at line " + std::to_string(peek().line));
    }
}

// -------------------- Expressions --------------------
std::shared_ptr<Node> Parser::condition() {
    auto condNode = std::make_shared<Node>(NodeType::CONDITION);

    auto leftExpr = expr();
    if (leftExpr) condNode->addChild(leftExpr);

    if (check(TokenType::EQUAL_EQ) || check(TokenType::BANG_EQ) ||
        check(TokenType::LESS) || check(TokenType::LESS_EQ) ||
        check(TokenType::GREATER) || check(TokenType::GREATER_EQ)) {
        Token opToken = peek();
        advance();
        condNode->addChild(std::make_shared<Node>(NodeType::TOKEN, opToken, opToken.lexeme));
    } else {
        throw std::runtime_error("Expected relational operator in condition");
    }

    auto rightExpr = expr();
    if (rightExpr) condNode->addChild(rightExpr);

    return condNode;
}

std::shared_ptr<Node> Parser::expr() {
    auto exprNode = std::make_shared<Node>(NodeType::EXPRESSION);

    auto termNode = term();
    if (termNode) exprNode->addChild(termNode);

    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        Token opToken = peek();
        advance();
        exprNode->addChild(std::make_shared<Node>(NodeType::TOKEN, opToken, opToken.lexeme));

        auto nextTerm = term();
        if (nextTerm) exprNode->addChild(nextTerm);
    }

    return exprNode;
}

std::shared_ptr<Node> Parser::term() {
    auto termNode = std::make_shared<Node>(NodeType::TERM);

    auto factorNode = factor();
    if (factorNode) termNode->addChild(factorNode);

    while (check(TokenType::STARR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
        Token opToken = peek();
        advance();
        termNode->addChild(std::make_shared<Node>(NodeType::TOKEN, opToken, opToken.lexeme));

        auto nextFactor = factor();
        if (nextFactor) termNode->addChild(nextFactor);
    }

    return termNode;
}

std::shared_ptr<Node> Parser::factor() {
    if (check(TokenType::NUMBER)) {
        Token numToken = peek();
        advance();
        return std::make_shared<Node>(NodeType::FACTOR, numToken, numToken.literal);
    }
    if (check(TokenType::IDENTIFIER)) {
        Token idToken = peek();
        advance();
        return std::make_shared<Node>(NodeType::FACTOR, idToken, idToken.lexeme);
    }
    if (check(TokenType::STAR)) {
        Token starToken = peek();
        advance();
        return std::make_shared<Node>(NodeType::FACTOR, starToken, starToken.literal);
    }
    if (check(TokenType::STARLIGHT)) {
        Token trueToken = peek();
        advance();
        return std::make_shared<Node>(NodeType::FACTOR, trueToken, "true");
    }
    if (check(TokenType::VOIDNESS)) {
        Token falseToken = peek();
        advance();
        return std::make_shared<Node>(NodeType::FACTOR, falseToken, "false");
    }
    if (match(TokenType::LEFT_PAREN)) {
        auto lpToken = previous();
        auto exprNode = expr();
        Token rpToken = peek();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");

        auto factorNode = std::make_shared<Node>(NodeType::FACTOR);
        factorNode->addChild(std::make_shared<Node>(NodeType::TOKEN, lpToken));
        if (exprNode) factorNode->addChild(exprNode);
        factorNode->addChild(std::make_shared<Node>(NodeType::TOKEN, rpToken));
        return factorNode;
    }

    throw std::runtime_error("Expected expression factor, got: " + peek().lexeme);
}

std::shared_ptr<Node> Parser::exprList() {
    auto listNode = std::make_shared<Node>(NodeType::EXPRESSION_LIST);

    auto exprNode = expr();
    if (exprNode) listNode->addChild(exprNode);

    while (match(TokenType::COMMA)) {
        listNode->addChild(std::make_shared<Node>(NodeType::TOKEN, previous(), ","));
        auto nextExpr = expr();
        if (nextExpr) listNode->addChild(nextExpr);
    }

    return listNode;
}

std::shared_ptr<Node> Parser::exprOpt() {
    if (!check(TokenType::SEMICOLON)) {
        return expr();
    }
    return nullptr;
}

// -------------------- Helpers --------------------
const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

void Parser::consume(TokenType type, const std::string& errorMessage) {
    if (check(type)) {
        advance();
    } else {
        throw std::runtime_error(errorMessage + " at line " + std::to_string(peek().line));
    }
}

bool Parser::isFunctionDeclaration() {
    // Look ahead to determine if this is a function declaration
    // Pattern: Type Identifier (
    int saved = current;

    // Skip type
    advance();

    // Check for identifier
    if (!check(TokenType::IDENTIFIER)) {
        current = saved;
        return false;
    }
    advance();

    // Check for opening parenthesis
    bool isFuncDecl = check(TokenType::LEFT_PAREN);

    current = saved;
    return isFuncDecl;
}