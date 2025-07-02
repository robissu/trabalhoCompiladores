
#include "semantic.h"
#include <iostream>

SemanticAnalyzer::SemanticAnalyzer(SymbolTable& table) : symbolTable(table) {}

void SemanticAnalyzer::error(const std::string& message, int line) {
    errorMessage = "Erro semantico";
    if (line > 0) {
        errorMessage += " na linha " + std::to_string(line);
    }
    errorMessage += ": " + message;
}

bool SemanticAnalyzer::analyze(ASTNodePtr root) {
    if (!root) return false;
    
    errorMessage.clear();
    
    // Análise das declarações
    for (auto child : root->children) {
        if (child->type == NodeType::DECLARACAO) {
            analyzeDeclarations(child);
        } else if (child->type == NodeType::LISTA_COMANDOS) {
            analyzeCommands(child);
        }
    }
    
    return !hasError();
}

void SemanticAnalyzer::analyzeDeclarations(ASTNodePtr node) {
    if (!node) return;
    
    // Agora processa cada declaração individual dentro do nó DECLARACAO
    for (auto decl : node->children) {
        if (decl->children.size() < 2) continue;
        
        auto listaVar = decl->children[0];
        auto tipo = decl->children[1];
        
        SymbolType symbolType = (tipo->token.type == TokenType::INTEIRO) ? 
                               SymbolType::INTEIRO : SymbolType::LOGICO;
        
        // Processa cada variável na lista
        for (auto var : listaVar->children) {
            if (!symbolTable.declare(var->token.value, symbolType)) {
                error("Variável '" + var->token.value + "' já foi declarada", var->token.line);
                return;
            }
        }
    }
}
void SemanticAnalyzer::analyzeCommands(ASTNodePtr node) {
    if (!node) return;
    
    for (auto cmd : node->children) {
        analyzeCommand(cmd);
        if (hasError()) return;
    }
}

void SemanticAnalyzer::analyzeCommand(ASTNodePtr node) {
    if (!node) return;
    
    switch (node->type) {
        case NodeType::ATRIBUICAO:
            analyzeAssignment(node);
            break;
        case NodeType::SE:
            analyzeIf(node);
            break;
        case NodeType::ENQUANTO:
            analyzeWhile(node);
            break;
        case NodeType::LER:
            analyzeRead(node);
            break;
        case NodeType::ESCREVER:
            analyzeWrite(node);
            break;
        default:
            break;
    }
}

void SemanticAnalyzer::analyzeAssignment(ASTNodePtr node) {
    if (!node || node->children.size() < 2) return;
    
    auto var = node->children[0];
    auto expr = node->children[1];
    
    if (!symbolTable.exists(var->token.value)) {
        error("Variável '" + var->token.value + "' não foi declarada", var->token.line);
        return;
    }
    
    Symbol* symbol = symbolTable.get(var->token.value);
    SymbolType exprType = getExpressionType(expr);
    
    if (symbol->type != exprType) {
        error("Tipos incompativeis na atribuicao", var->token.line);
    }
}

void SemanticAnalyzer::analyzeIf(ASTNodePtr node) {
    if (!node || node->children.empty()) return;
    
    auto condition = node->children[0];
    SymbolType condType = getExpressionType(condition);
    
    if (condType != SymbolType::INTEIRO) {
        error("condicao do 'se' deve ser do tipo inteiro", condition->token.line);
        return;
    }
    
    // Analisa comandos then e else
    for (size_t i = 1; i < node->children.size(); i++) {
        analyzeCommand(node->children[i]);
    }
}

void SemanticAnalyzer::analyzeWhile(ASTNodePtr node) {
    if (!node || node->children.empty()) return;
    
    auto condition = node->children[0];
    SymbolType condType = getExpressionType(condition);
    
    if (condType != SymbolType::INTEIRO) {
        error("condicao do 'enquanto' deve ser do tipo inteiro", condition->token.line);
        return;
    }
    
    // Analisa comando do corpo
    if (node->children.size() > 1) {
        analyzeCommand(node->children[1]);
    }
}

void SemanticAnalyzer::analyzeRead(ASTNodePtr node) {
    if (!node) return;
    
    for (auto var : node->children) {
        if (!symbolTable.exists(var->token.value)) {
            error("Variável '" + var->token.value + "' não foi declarada", var->token.line);
            return;
        }
    }
}

void SemanticAnalyzer::analyzeWrite(ASTNodePtr node) {
    if (!node) return;
    
    for (auto expr : node->children) {
        getExpressionType(expr); // Verifica se a expressão é válida
        if (hasError()) return;
    }
}

SymbolType SemanticAnalyzer::getExpressionType(ASTNodePtr node) {
    if (!node) return SymbolType::INTEIRO;
    
    switch (node->type) {
        case NodeType::NUMERO:
            return SymbolType::INTEIRO;
            
        case NodeType::LITERAL:
            if (node->token.type == TokenType::VERDADEIRO || 
                node->token.type == TokenType::FALSO) {
                return SymbolType::LOGICO;
            }
            return SymbolType::INTEIRO;
            
        case NodeType::IDENTIFICADOR: {
            if (!symbolTable.exists(node->token.value)) {
                error("Variável '" + node->token.value + "' não foi declarada", node->token.line);
                return SymbolType::INTEIRO;
            }
            Symbol* symbol = symbolTable.get(node->token.value);
            return symbol->type;
        }
        
        case NodeType::BINARIO: {
            if (node->children.size() < 2) return SymbolType::INTEIRO;
            
            SymbolType leftType = getExpressionType(node->children[0]);
            SymbolType rightType = getExpressionType(node->children[1]);
            
            // Operadores relacionais sempre retornam inteiro (0 ou 1)
            if (node->token.type == TokenType::IGUAL || 
                node->token.type == TokenType::DIFERENTE ||
                node->token.type == TokenType::MENOR ||
                node->token.type == TokenType::MENOR_IGUAL ||
                node->token.type == TokenType::MAIOR ||
                node->token.type == TokenType::MAIOR_IGUAL) {
                return SymbolType::INTEIRO;
            }
            
            // Operadores aritméticos
            if (leftType != rightType) {
                error("Tipos incompativeis na operação", node->token.line);
            }
            
            return leftType;
        }
        
        case NodeType::UNARIO: {
            if (node->children.empty()) return SymbolType::INTEIRO;
            return getExpressionType(node->children[0]);
        }
        
        default:
            return SymbolType::INTEIRO;
    }
}
