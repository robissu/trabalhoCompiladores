// src/Lexer/Lexer.cpp

#include "Lexer.h"
#include <cctype> // Para isalpha, isdigit, isalnum
#include <iostream>
#include <sstream> // Para std::ostringstream

// Construtor
Lexer::Lexer(const std::string& source)
    : source(source), position(0), line(1), column(1) {}

// Obtém o próximo caractere sem avançar a posição
char Lexer::peek() {
    if (position >= source.length()) {
        return '\0'; // Fim do arquivo
    }
    return source[position];
}

// Consome o caractere atual e avança a posição
char Lexer::advance() {
    if (position >= source.length()) {
        return '\0'; // Fim do arquivo
    }
    char c = source[position++];
    if (c == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return c;
}

// Reporta um erro léxico
void Lexer::lexicalError(const std::string& message) {
    std::ostringstream oss;
    oss << "Erro Lexico na linha " << line << ", coluna " << column << ": " << message;
    throw LexicalError(oss.str(), line, column);
}

// Ignora espaços em branco, quebras de linha e comentários
void Lexer::skipWhitespaceAndComments() {
    while (position < source.length()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            advance();
        } else if (c == '/' && position + 1 < source.length() && source[position + 1] == '/') {
            // É um comentário de linha (//). Pular até o final da linha.
            while (position < source.length() && peek() != '\n' && peek() != '\r') {
                advance();
            }
            // Pular o próprio '\n' ou '\r' para que a próxima chamada não o processe
            if (peek() == '\n' || peek() == '\r') {
                advance();
            }
        } else if (c == '{') {
            // É um comentário de bloco {}. Pular até '}'
            advance(); // Consome '{'
            while (position < source.length() && peek() != '}') {
                advance();
            }
            if (peek() == '}') {
                advance(); // Consome '}'
            } else {
                lexicalError("Comentario de bloco nao fechado.");
            }
        }
        else {
            break; // Não é espaço em branco nem comentário, pare de pular
        }
    }
}

// Obtém o próximo token do código fonte
Token Lexer::getNextToken() {
    skipWhitespaceAndComments();

    if (position >= source.length()) {
        return Token(TokenType::END_OF_FILE, "EOF", line, column);
    }

    char c = peek();
    int startLine = line;
    int startColumn = column;

    // Identificadores e Palavras-chave
    if (isalpha(c)) {
        std::string lexeme;
        while (isalnum(peek())) {
            lexeme += advance();
        }
        TokenType type = getKeywordType(lexeme);
        return Token(type, lexeme, startLine, startColumn);
    }

    // Números
    if (isdigit(c)) {
        std::string lexeme;
        while (isdigit(peek())) {
            lexeme += advance();
        }
        return Token(TokenType::NUMBER, lexeme, startLine, startColumn);
    }

    // Literais de string
    if (c == '\'') {
        advance(); // Consome o primeiro apóstrofo
        std::string lexeme;
        while (peek() != '\'' && position < source.length()) {
            if (peek() == '\n' || peek() == '\r') {
                lexicalError("Literal de string nao fechado (quebra de linha inesperada).");
            }
            lexeme += advance();
        }
        if (peek() == '\'') {
            advance(); // Consome o segundo apóstrofo
            return Token(TokenType::STRING_LITERAL, lexeme, startLine, startColumn);
        } else {
            lexicalError("Literal de string nao fechado.");
        }
    }

    // Operadores e Delimitadores de dois caracteres
    if (c == ':') {
        if (position + 1 < source.length() && source[position + 1] == '=') {
            advance(); // Consome ':'
            advance(); // Consome '='
            return Token(TokenType::OP_ASSIGN, ":=", startLine, startColumn);
        }
    } else if (c == '<') {
        if (position + 1 < source.length() && source[position + 1] == '=') {
            advance(); advance(); return Token(TokenType::OP_LE, "<=", startLine, startColumn);
        } else if (position + 1 < source.length() && source[position + 1] == '>') {
            advance(); advance(); return Token(TokenType::OP_NE, "<>", startLine, startColumn);
        }
    } else if (c == '>') {
        if (position + 1 < source.length() && source[position + 1] == '=') {
            advance(); advance(); return Token(TokenType::OP_GE, ">=", startLine, startColumn);
        }
    }

    // Operadores e Delimitadores de um caractere
    switch (c) {
        case '+': advance(); return Token(TokenType::OP_PLUS, "+", startLine, startColumn);
        case '-': advance(); return Token(TokenType::OP_MINUS, "-", startLine, startColumn);
        case '*': advance(); return Token(TokenType::OP_MULT, "*", startLine, startColumn);
        case '/': advance(); return Token(TokenType::OP_DIV, "/", startLine, startColumn); // Continua a tratar / sozinho como DIV
        case '=': advance(); return Token(TokenType::OP_EQ, "=", startLine, startColumn);
        case '<': advance(); return Token(TokenType::OP_LT, "<", startLine, startColumn);
        case '>': advance(); return Token(TokenType::OP_GT, ">", startLine, startColumn);
        case ';': advance(); return Token(TokenType::DELIM_SEMICOLON, ";", startLine, startColumn);
        case '.': advance(); return Token(TokenType::DELIM_DOT, ".", startLine, startColumn);
        case ',': advance(); return Token(TokenType::DELIM_COMMA, ",", startLine, startColumn);
        case ':': advance(); return Token(TokenType::DELIM_COLON, ":", startLine, startColumn);
        case '(': advance(); return Token(TokenType::DELIM_LPAREN, "(", startLine, startColumn);
        case ')': advance(); return Token(TokenType::DELIM_RPAREN, ")", startLine, startColumn);
        default:
            // Caractere não reconhecido
            // Certifique-se de que o caractere que causou o erro é avançado para evitar loop infinito
            char unknownChar = advance();
            lexicalError("Caractere nao reconhecido: '" + std::string(1, unknownChar) + "'");
            return Token(TokenType::UNKNOWN, std::string(1, unknownChar), startLine, startColumn); // Retorna UNKNOWN para continuar o processamento
    }
}

// Mapeia lexemas para tipos de palavras-chave
TokenType Lexer::getKeywordType(const std::string& lexeme) {
    if (lexeme == "programa") return TokenType::KW_PROGRAMA;
    if (lexeme == "var") return TokenType::KW_VAR;
    if (lexeme == "inteiro") return TokenType::KW_INTEIRO;
    if (lexeme == "logico") return TokenType::KW_LOGICO;
    if (lexeme == "inicio") return TokenType::KW_INICIO;
    if (lexeme == "fim") return TokenType::KW_FIM;
    if (lexeme == "se") return TokenType::KW_SE;
    if (lexeme == "entao") return TokenType::KW_ENTAO;
    if (lexeme == "senao") return TokenType::KW_SENAO;
    if (lexeme == "enquanto") return TokenType::KW_ENQUANTO;
    if (lexeme == "faca") return TokenType::KW_FACA;
    if (lexeme == "ler") return TokenType::KW_LER;
    if (lexeme == "escrever") return TokenType::KW_ESCREVER;
    if (lexeme == "verdadeiro") return TokenType::KW_VERDADEIRO;
    if (lexeme == "falso") return TokenType::KW_FALSO;
    if (lexeme == "e") return TokenType::KW_E;
    if (lexeme == "ou") return TokenType::KW_OU;
    return TokenType::ID; // Se não for uma palavra-chave, é um identificador
}

// Implementação do construtor da exceção LexicalError (já estava correto)
LexicalError::LexicalError(const std::string& message, int line, int column)
    : std::runtime_error(message), line(line), column(column) {}

int LexicalError::getLine() const {
    return line;
}

int LexicalError::getColumn() const {
    return column;
}