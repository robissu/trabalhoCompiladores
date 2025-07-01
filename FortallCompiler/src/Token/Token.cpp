// src/Token/Token.cpp

#include "Token.h"
#include <map>
#include <sstream>
#include <stdexcept>

// Mapeamento estático de TokenType para nomes de string
static const std::map<TokenType, std::string> tokenTypeNames = {
    // Palavras-chave
    {TokenType::KW_PROGRAMA, "KW_PROGRAMA"}, {TokenType::KW_VAR, "KW_VAR"}, {TokenType::KW_INICIO, "KW_INICIO"},
    {TokenType::KW_FIM, "KW_FIM"}, {TokenType::KW_ENQUANTO, "KW_ENQUANTO"}, {TokenType::KW_FACA, "KW_FACA"},
    {TokenType::KW_SE, "KW_SE"}, {TokenType::KW_ENTAO, "KW_ENTAO"}, {TokenType::KW_SENAO, "KW_SENAO"},
    {TokenType::KW_VERDADEIRO, "KW_VERDADEIRO"}, {TokenType::KW_FALSO, "KW_FALSO"},
    {TokenType::KW_INTEIRO, "KW_INTEIRO"}, {TokenType::KW_LOGICO, "KW_LOGICO"},
    {TokenType::KW_LER, "KW_LER"}, {TokenType::KW_ESCREVER, "KW_ESCREVER"},
    {TokenType::KW_OU, "KW_OU"}, {TokenType::KW_E, "KW_E"},

    // Operadores
    {TokenType::OP_PLUS, "OP_PLUS"}, {TokenType::OP_MINUS, "OP_MINUS"}, {TokenType::OP_MULT, "OP_MULT"},
    {TokenType::OP_DIV, "OP_DIV"}, {TokenType::OP_EQ, "OP_EQ"}, {TokenType::OP_NE, "OP_NE"},
    {TokenType::OP_LT, "OP_LT"}, {TokenType::OP_LE, "OP_LE"}, {TokenType::OP_GT, "OP_GT"},
    {TokenType::OP_GE, "OP_GE"}, {TokenType::OP_ASSIGN, "OP_ASSIGN"},

    // Delimitadores
    {TokenType::DELIM_LPAREN, "DELIM_LPAREN"}, {TokenType::DELIM_RPAREN, "DELIM_RPAREN"},
    {TokenType::DELIM_LBRACKET, "DELIM_LBRACKET"}, {TokenType::DELIM_RBRACKET, "DELIM_RBRACKET"},
    {TokenType::DELIM_DOT, "DELIM_DOT"}, {TokenType::DELIM_COMMA, "DELIM_COMMA"},
    {TokenType::DELIM_COLON, "DELIM_COLON"}, {TokenType::DELIM_SEMICOLON, "DELIM_SEMICOLON"},

    // Literais e Identificadores
    {TokenType::ID, "ID"}, {TokenType::NUMBER, "NUMBER"}, {TokenType::STRING_LITERAL, "STRING_LITERAL"},

    // Fim de arquivo e Desconhecido
    {TokenType::END_OF_FILE, "END_OF_FILE"}, {TokenType::UNKNOWN, "UNKNOWN"}
};

// Construtor
Token::Token(TokenType type, const std::string& lexeme, int line, int column)
    : type(type), lexeme(lexeme), line(line), column(column)
{
    // O corpo pode ficar vazio se não houver lógica adicional.
}

// Métodos getter
TokenType Token::getType() const {
    return type;
}

std::string Token::getLexeme() const {
    return lexeme;
}

int Token::getLine() const {
    return line;
}

int Token::getColumn() const {
    return column;
}

// Implementação de toString
std::string Token::toString() const {
    std::ostringstream oss;
    auto it = tokenTypeNames.find(type);
    if (it != tokenTypeNames.end()) {
        oss << "Token: " << it->second;
    } else {
        oss << "Token: UNKNOWN_TYPE";
    }

    if (type == TokenType::ID || type == TokenType::NUMBER || type == TokenType::STRING_LITERAL || type == TokenType::UNKNOWN) {
        oss << "(" << lexeme << ")";
    }
    oss << " (Linha: " << line << ", Coluna: " << column << ")";
    return oss.str();
}