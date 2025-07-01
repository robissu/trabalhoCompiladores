// src/Parser/Parser.h

#ifndef PARSER_H
#define PARSER_H

#include "../Lexer/Lexer.h"
#include "../Token/Token.h"
#include <string>
#include <stdexcept> // Para std::runtime_error

// Exceção personalizada para erros sintáticos
class SyntaxError : public std::runtime_error {
private:
    int line;
    int column;

public:
    // Construtor
    SyntaxError(const std::string& message, int line, int column);

    // Métodos para obter linha e coluna do erro
    int getLine() const;
    int getColumn() const;
};

// Classe Parser
class Parser {
private:
    Lexer& lexer;
    Token currentToken;

    // Métodos auxiliares
    void match(TokenType expectedType, const std::string& errorMessage);
    void syntaxError(const std::string& message);

    // Regras da gramática (funções para cada não-terminal)
    void parsePrograma();
    void parseDeclaracaoVariaveis();
    void parseListaIds();
    void parseTipo();
    void parseBlocoComandos();
    void parseComando();
    void parseAtribuicao();
    void parseSe();
    void parseEnquanto();
    void parseLer();
    void parseEscrever();
    void parseExpressao();
    void parseExpressaoLogica();
    void parseExpressaoRelacional();
    void parseExpressaoAditiva();
    void parseExpressaoMultiplicativa();
    void parseFator();

public:
    // Construtor
    Parser(Lexer& lexer);

    // Método principal para iniciar a análise
    void parse();
};

#endif // PARSER_H