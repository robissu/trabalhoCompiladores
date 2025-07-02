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
        
        // Processa cada variavel na lista
        for (auto var : listaVar->children) {
            if (!symbolTable.declare(var->token.value, symbolType)) {
                error("Variavel '" + var->token.value + "' ja foi declarada", var->token.line);
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
        error("Variavel '" + var->token.value + "' nao foi declarada", var->token.line);
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
    
    if (condType != SymbolType::LOGICO) { 
        error("Condicao do 'se' deve ser do tipo logico", condition->token.line);
        return;
    }
    
    for (size_t i = 1; i < node->children.size(); i++) {
        analyzeCommand(node->children[i]);
        if (hasError()) return;
    }
}

void SemanticAnalyzer::analyzeWhile(ASTNodePtr node) {
    if (!node || node->children.empty()) return;
    
    auto condition = node->children[0];
    SymbolType condType = getExpressionType(condition);
    
    if (condType != SymbolType::LOGICO) {
        error("Condicao do 'enquanto' deve ser do tipo logico", condition->token.line);
        return;
    }
    
    if (node->children.size() > 1) {
        analyzeCommand(node->children[1]);
        if (hasError()) return;
    }
}

void SemanticAnalyzer::analyzeRead(ASTNodePtr node) {
    if (!node || node->children.empty()) return;
    
    for (auto idNode : node->children) {
        if (idNode->type == NodeType::IDENTIFICADOR) {
            std::string varName = idNode->token.value;
            if (!symbolTable.exists(varName)) {
                error("Variavel '" + varName + "' nao foi declarada", idNode->token.line);
            }
        }
    }
}

void SemanticAnalyzer::analyzeWrite(ASTNodePtr node) {
    if (!node || node->children.empty()) return;
    
    for (auto expr : node->children) {
        getExpressionType(expr);
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
            std::string varName = node->token.value;
            if (!symbolTable.exists(varName)) {
                error("Variavel '" + varName + "' nao foi declarada", node->token.line);
                return SymbolType::INTEIRO;
            }
            Symbol* symbol = symbolTable.get(varName);
            // === REMOÇÃO: A verificação de inicialização foi movida para o interpretador. ===
            // if (!symbol->initialized) {
            //     error("Variavel '" + varName + "' nao foi inicializada", node->token.line);
            // }
            // ==============================================================================
            return symbol->type;
        }
        
        case NodeType::BINARIO: {
            if (node->children.size() < 2) return SymbolType::INTEIRO;
            
            SymbolType leftType = getExpressionType(node->children[0]);
            SymbolType rightType = getExpressionType(node->children[1]);
            
            if (node->token.type == TokenType::IGUAL || 
                node->token.type == TokenType::DIFERENTE ||
                node->token.type == TokenType::MENOR ||
                node->token.type == TokenType::MENOR_IGUAL ||
                node->token.type == TokenType::MAIOR ||
                node->token.type == TokenType::MAIOR_IGUAL) {
                
                if (leftType != SymbolType::INTEIRO || rightType != SymbolType::INTEIRO) {
                    error("Operadores relacionais esperam operandos inteiros", node->token.line);
                }
                return SymbolType::LOGICO;
            }
            
            if (node->token.type == TokenType::MAIS ||
                node->token.type == TokenType::MENOS ||
                node->token.type == TokenType::MULTIPLICACAO ||
                node->token.type == TokenType::DIVISAO) {
                
                if (leftType != SymbolType::INTEIRO || rightType != SymbolType::INTEIRO) {
                    error("Operadores aritmeticos esperam operandos inteiros", node->token.line);
                    return SymbolType::INTEIRO;
                }
                return SymbolType::INTEIRO;
            }
            
            return leftType;
        }
        
        case NodeType::UNARIO: {
            if (node->children.empty()) return SymbolType::INTEIRO;
            SymbolType operandType = getExpressionType(node->children[0]);
            if (node->token.type == TokenType::MENOS) {
                if (operandType != SymbolType::INTEIRO) {
                    error("Operador unario '-' espera operando inteiro", node->token.line);
                    return SymbolType::INTEIRO;
                }
                return SymbolType::INTEIRO;
            }
            return operandType;
        }
        
        default:
            return SymbolType::INTEIRO;
    }
}