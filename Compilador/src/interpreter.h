
#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "symbol_table.h"
#include <variant>
#include <iostream>

class Interpreter {
private:
    SymbolTable& symbolTable;
    std::string errorMessage;
    
    void error(const std::string& message);
    std::variant<int, bool> evaluateExpression(ASTNodePtr node);
    void executeCommand(ASTNodePtr node);
    void executeAssignment(ASTNodePtr node);
    void executeIf(ASTNodePtr node);
    void executeWhile(ASTNodePtr node);
    void executeRead(ASTNodePtr node);
    void executeWrite(ASTNodePtr node);
    void executeCommands(ASTNodePtr node);
    
public:
    Interpreter(SymbolTable& table);
    bool execute(ASTNodePtr root);
    bool hasError() const { return !errorMessage.empty(); }
    const std::string& getError() const { return errorMessage; }
};

#endif
