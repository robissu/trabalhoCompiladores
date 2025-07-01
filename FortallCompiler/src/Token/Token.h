// src/Token/Token.h
#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum class TokenType {
    // Palavras-chave
    KW_PROGRAMA,
    KW_VAR,
    KW_INICIO,
    KW_FIM,
    KW_ENQUANTO,
    KW_FACA,
    KW_SE,
    KW_ENTAO,
    KW_SENAO,
    KW_VERDADEIRO,
    KW_FALSO,
    KW_INTEIRO,
    KW_LOGICO,
    KW_LER,
    KW_ESCREVER,
    KW_OU,
    KW_E,

    // Identificadores e Literais
    ID,
    NUMBER,
    STRING_LITERAL,

    // Operadores Aritméticos
    OP_PLUS,    // +
    OP_MINUS,   // -
    OP_MULT,    // *
    OP_DIV,     // /

    // Operadores Relacionais
    OP_EQ,      // =
    OP_NE,      // <>
    OP_LT,      // <
    OP_LE,      // <=
    OP_GT,      // >
    OP_GE,      // >=

    // Operador de Atribuição
    OP_ASSIGN,  // :=

    // Delimitadores
    DELIM_LPAREN,   // (
    DELIM_RPAREN,   // )
    DELIM_LBRACKET, // [ (Adicionado, caso seja usado no futuro)
    DELIM_RBRACKET, // ] (Adicionado, caso seja usado no futuro)
    DELIM_DOT,      // .
    DELIM_COMMA,    // ,
    DELIM_COLON,    // :
    DELIM_SEMICOLON,// ;

    // Fim de Arquivo e Erros
    END_OF_FILE,
    UNKNOWN     // Para caracteres não reconhecidos
};

class Token {
private:
    TokenType type;
    std::string lexeme;
    int line;
    int column;

public:
    // MUDANÇA AQUI: Adicionado 'const' e '&' para 'lexeme'
    Token(TokenType type, const std::string& lexeme, int line, int column);

    // Métodos Getters
    TokenType getType() const { return type; }
    std::string getLexeme() const { return lexeme; }
    int getLine() const { return line; }
    int getColumn() const { return column; }

    std::string toString() const;
};

#endif // TOKEN_H