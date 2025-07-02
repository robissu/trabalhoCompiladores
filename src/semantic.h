
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"
#include <string>

class SemanticAnalyzer {
private:
    SymbolTable& symbolTable;
    std::string errorMessage;
    
    void error(const std::string& message, int line = 0);
    SymbolType getExpressionType(ASTNodePtr node);
    void analyzeDeclarations(ASTNodePtr node);
    void analyzeCommands(ASTNodePtr node);
    void analyzeCommand(ASTNodePtr node);
    void analyzeAssignment(ASTNodePtr node);
    void analyzeIf(ASTNodePtr node);
    void analyzeWhile(ASTNodePtr node);
    void analyzeRead(ASTNodePtr node);
    void analyzeWrite(ASTNodePtr node);
    
public:
    SemanticAnalyzer(SymbolTable& table);
    bool analyze(ASTNodePtr root);
    bool hasError() const { return !errorMessage.empty(); }
    const std::string& getError() const { return errorMessage; }
};

#endif
