// src/Lexer/Lexer.h

#ifndef LEXER_H
#define LEXER_H

#include "../Token/Token.h" // Inclui a definição de Token e TokenType
#include <string>
#include <stdexcept> // Para std::runtime_error

// Exceção personalizada para erros léxicos
class LexicalError : public std::runtime_error {
private:
    int line;
    int column;

public:
    // Construtor que aceita mensagem, linha e coluna
    LexicalError(const std::string& message, int line, int column);

    // Métodos para obter a linha e a coluna do erro
    int getLine() const;
    int getColumn() const;
};

// Classe do Analisador Léxico (Lexer)
class Lexer {
private:
    std::string source; // Código fonte de entrada
    int position;       // Posição atual no código fonte
    int line;           // Linha atual no código fonte
    int column;         // Coluna atual na linha

    // Métodos auxiliares
    char peek();                                  // Olha o próximo caractere sem avançar
    char advance();                               // Avança para o próximo caractere
    void skipWhitespaceAndComments();             // Pula espaços em branco e comentários
    void lexicalError(const std::string& message); // Reporta um erro léxico
    TokenType getKeywordType(const std::string& lexeme); // Mapeia lexemas para palavras-chave

public:
    // Construtor
    Lexer(const std::string& source);

    // Método principal para obter o próximo token
    Token getNextToken();
};

#endif // LEXER_H