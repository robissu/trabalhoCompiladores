
#include "interpreter.h"
#include <iostream>
#include <sstream>
#include <limits>

Interpreter::Interpreter(SymbolTable& table) : symbolTable(table) {}

void Interpreter::error(const std::string& message) {
    errorMessage = "Erro de execucao: " + message;
}

bool Interpreter::execute(ASTNodePtr root) {
    if (!root) return false;
    
    errorMessage.clear();
    
    // Executa os comandos do programa
    for (auto child : root->children) {
        if (child->type == NodeType::LISTA_COMANDOS) {
            executeCommands(child);
            break;
        }
    }
    
    return !hasError();
}

void Interpreter::executeCommands(ASTNodePtr node) {
    if (!node) return;
    
    for (auto cmd : node->children) {
        executeCommand(cmd);
        if (hasError()) return;
    }
}

void Interpreter::executeCommand(ASTNodePtr node) {
    if (!node) return;
    
    switch (node->type) {
        case NodeType::ATRIBUICAO:
            executeAssignment(node);
            break;
        case NodeType::SE:
            executeIf(node);
            break;
        case NodeType::ENQUANTO:
            executeWhile(node);
            break;
        case NodeType::LER:
            executeRead(node);
            break;
        case NodeType::ESCREVER:
            executeWrite(node);
            break;
        case NodeType::LISTA_COMANDOS:
            executeCommands(node);
            break;
        default:
            break;
    }
}

void Interpreter::executeAssignment(ASTNodePtr node) {
    if (!node || node->children.size() < 2) return;
    
    auto var = node->children[0];
    auto expr = node->children[1];
    
    auto value = evaluateExpression(expr);
    symbolTable.assign(var->token.value, value);
}

void Interpreter::executeIf(ASTNodePtr node) {
    if (!node || node->children.empty()) return;
    
    auto condition = node->children[0];
    auto condValue = evaluateExpression(condition);
    
    int condInt = std::holds_alternative<int>(condValue) ? 
                  std::get<int>(condValue) : 
                  (std::get<bool>(condValue) ? 1 : 0);
    
    if (condInt != 0) {
        // Executa comando then
        if (node->children.size() > 1) {
            executeCommand(node->children[1]);
        }
    } else {
        // Executa comando else se existir
        if (node->children.size() > 2) {
            executeCommand(node->children[2]);
        }
    }
}

void Interpreter::executeWhile(ASTNodePtr node) {
    if (!node || node->children.size() < 2) return;
    
    auto condition = node->children[0];
    auto body = node->children[1];
    
    int loopCount = 0;
    const int MAX_ITERATIONS = 100000; // Proteção contra loop infinito
    
    while (loopCount < MAX_ITERATIONS) {

        //std::cout << "DEBUG: loop " << loopCount << std::endl;
       // std::cout << "DEBUG: Antes de avaliar, contador = " << /* valor de contador do ambiente */ << std::endl;
        //std::cout << "DEBUG: Antes de avaliar, limite = " << /* valor de limite do ambiente */ << std::endl;


        auto condValue = evaluateExpression(condition);
        int condInt = std::holds_alternative<int>(condValue) ? 
                      std::get<int>(condValue) : 
                      (std::get<bool>(condValue) ? 1 : 0);
        
        //std::cout << "DEBUG: condInt = " << condInt << std::endl;

        if (condInt == 0) break;
        
        executeCommand(body);

        //std::cout << "DEBUG: apos executeCommand, contador = " << /* novo valor de contador do ambiente */ << std::endl;

        if (hasError()) break;
        
        loopCount++;
    }
    
    if (loopCount >= MAX_ITERATIONS) {
        error("Loop infinito detectado - interrompendo execucao");
    }
}

void Interpreter::executeRead(ASTNodePtr node) {
    if (!node) return;
    
    for (auto var : node->children) {
        Symbol* symbol = symbolTable.get(var->token.value);
        if (!symbol) continue;
        
        std::cout << "Digite o valor para " << var->token.value << ": ";
        std::cout.flush();
        
        if (symbol->type == SymbolType::INTEIRO) {
            int value;
            if (std::cin >> value) {
                symbolTable.assign(var->token.value, value);
                // Limpa o buffer apos leitura bem-sucedida
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            } else {
                // Trata erro de entrada
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                error("Entrada inválida para variável inteira '" + var->token.value + "'");
                return;
            }
        } else {
            std::string input;
            std::cin.ignore(); // Ignora o newline pendente
            std::getline(std::cin, input);
            bool value = (input == "verdadeiro" || input == "true" || input == "1");
            symbolTable.assign(var->token.value, value);
        }
    }
}

void Interpreter::executeWrite(ASTNodePtr node) {
    if (!node) return;
    
    for (size_t i = 0; i < node->children.size(); i++) {
        if (i > 0) std::cout << " ";
        
        auto expr = node->children[i];
        
        if (expr->type == NodeType::STRING_LITERAL) {
            std::cout << expr->token.value;
        } else {
            auto value = evaluateExpression(expr);
            
            if (std::holds_alternative<int>(value)) {
                std::cout << std::get<int>(value);
            } else {
                std::cout << (std::get<bool>(value) ? "verdadeiro" : "falso");
            }
        }
    }
    std::cout << std::endl;
}

std::variant<int, bool> Interpreter::evaluateExpression(ASTNodePtr node) {
    if (!node) return 0;
    
    switch (node->type) {
        case NodeType::NUMERO:
            return std::stoi(node->token.value);
            
        case NodeType::LITERAL:
            if (node->token.type == TokenType::VERDADEIRO) {
                return true;
            } else if (node->token.type == TokenType::FALSO) {
                return false;
            }
            return 0;
            
        case NodeType::IDENTIFICADOR: {
            Symbol* symbol = symbolTable.get(node->token.value);
            if (!symbol || !symbol->initialized) {
                error("Variável '" + node->token.value + "' nao foi inicializada");
                return 0;
            }
            return symbol->value;
        }
        
        case NodeType::BINARIO: {
            if (node->children.size() < 2) return 0;
            
            auto left = evaluateExpression(node->children[0]);
            auto right = evaluateExpression(node->children[1]);
            
            int leftInt = std::holds_alternative<int>(left) ? 
                         std::get<int>(left) : 
                         (std::get<bool>(left) ? 1 : 0);
            int rightInt = std::holds_alternative<int>(right) ? 
                          std::get<int>(right) : 
                          (std::get<bool>(right) ? 1 : 0);
            
            switch (node->token.type) {
                case TokenType::MAIS:
                    return leftInt + rightInt;
                case TokenType::MENOS:
                    return leftInt - rightInt;
                case TokenType::MULTIPLICACAO:
                    return leftInt * rightInt;
                case TokenType::DIVISAO:
                    if (rightInt == 0) {
                        error("Divisão por zero");
                        return 0;
                    }
                    return leftInt / rightInt;
                case TokenType::IGUAL:
                    return (leftInt == rightInt) ? 1 : 0;
                case TokenType::DIFERENTE:
                    return (leftInt != rightInt) ? 1 : 0;
                case TokenType::MENOR:
                    return (leftInt < rightInt) ? 1 : 0;
                case TokenType::MENOR_IGUAL:
                    return (leftInt <= rightInt) ? 1 : 0;
                case TokenType::MAIOR:
                    return (leftInt > rightInt) ? 1 : 0;
                case TokenType::MAIOR_IGUAL:
                    return (leftInt >= rightInt) ? 1 : 0;
                default:
                    return 0;
            }
        }
        
        case NodeType::UNARIO: {
            if (node->children.empty()) return 0;
            
            auto operand = evaluateExpression(node->children[0]);
            int operandInt = std::holds_alternative<int>(operand) ? 
                            std::get<int>(operand) : 
                            (std::get<bool>(operand) ? 1 : 0);
            
            if (node->token.type == TokenType::MENOS) {
                return -operandInt;
            }
            return operandInt;
        }
        
        default:
            return 0;
    }
}
