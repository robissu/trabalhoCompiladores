
#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Palavras reservadas
    PROGRAMA, INICIO, FIM, VAR, INTEIRO, LOGICO,
    SE, ENTAO, SENAO, ENQUANTO, FACA,
    LER, ESCREVER, VERDADEIRO, FALSO,
    
    // Identificadores e literais
    IDENTIFICADOR, NUMERO, STRING,
    
    // Operadores aritméticos
    MAIS, MENOS, MULTIPLICACAO, DIVISAO,
    
    // Operadores relacionais
    IGUAL, DIFERENTE, MENOR, MENOR_IGUAL, MAIOR, MAIOR_IGUAL,
    
    // Operadores de atribuição
    ATRIBUICAO,
    
    // Delimitadores
    PONTO_VIRGULA, PONTO, VIRGULA, DOIS_PONTOS,
    PARENTESE_ESQ, PARENTESE_DIR, COLCHETE_ESQ, COLCHETE_DIR, FIM_ENQUANTO,
    
    // Especiais
    FIM_ARQUIVO, ERRO, COMENTARIO
};

struct Token {
    TokenType type;
    std::string value;
    int line;
    int column;
    
    Token(TokenType t = TokenType::ERRO, const std::string& v = "", int l = 1, int c = 1)
        : type(t), value(v), line(l), column(c) {}
};

std::string tokenTypeToString(TokenType type);

#endif
