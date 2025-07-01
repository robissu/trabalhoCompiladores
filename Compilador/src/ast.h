
#ifndef AST_H
#define AST_H

#include "token.h"
#include <vector>
#include <memory>

enum class NodeType {
    PROGRAMA, DECLARACAO, LISTA_VAR, TIPO,
    LISTA_COMANDOS, COMANDO, ATRIBUICAO,
    SE, ENQUANTO, LER, ESCREVER,
    EXPRESSAO, BINARIO, UNARIO, LITERAL,
    IDENTIFICADOR, NUMERO, STRING_LITERAL
};

struct ASTNode {
    NodeType type;
    Token token;
    std::vector<std::shared_ptr<ASTNode>> children;
    
    ASTNode(NodeType t, const Token& tok = Token())
        : type(t), token(tok) {}
    
    void addChild(std::shared_ptr<ASTNode> child) {
        children.push_back(child);
    }
};

using ASTNodePtr = std::shared_ptr<ASTNode>;

#endif
