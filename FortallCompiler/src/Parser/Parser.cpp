// src/Parser/Parser.cpp

#include "Parser.h"
#include <iostream>
#include <sstream>   // Para std::ostringstream
#include <stdexcept> // Para std::runtime_error

// Construtor da exceção SyntaxError
SyntaxError::SyntaxError(const std::string& message, int line, int column)
    : std::runtime_error(message), line(line), column(column) {} // Inicializa line e column aqui

// Implementação dos métodos getter para SyntaxError
int SyntaxError::getLine() const {
    return line;
}

int SyntaxError::getColumn() const {
    return column;
}

// Construtor do Parser
Parser::Parser(Lexer& lexer)
    : lexer(lexer), currentToken(lexer.getNextToken()) {
    // O construtor não precisa fazer mais nada além da inicialização dos membros.
}

// Método para comparar o token atual com o tipo esperado e avançar
void Parser::match(TokenType expectedType, const std::string& errorMessage) {
    if (currentToken.getType() == expectedType) {
        currentToken = lexer.getNextToken();
    } else {
        syntaxError(errorMessage + " (Token inesperado: " + currentToken.toString() + ")");
    }
}

// Método para reportar erros sintáticos
void Parser::syntaxError(const std::string& message) {
    std::ostringstream oss;
    oss << "Erro Sintatico na linha " << currentToken.getLine()
        << ", coluna " << currentToken.getColumn() << ": " << message;
    throw SyntaxError(oss.str(), currentToken.getLine(), currentToken.getColumn());
}

// Método principal para iniciar a análise sintática
void Parser::parse() {
    try {
        parsePrograma();
        if (currentToken.getType() != TokenType::END_OF_FILE) {
            syntaxError("Esperado fim de arquivo, mas encontrado token: " + currentToken.toString());
        }
    } catch (const SyntaxError& e) {
        std::cerr << "Erro de Parsing: " << e.what() << std::endl;
        // Opcional: Relatar linha e coluna do erro
    }
}

// Implementações das regras da gramática
// programa ::= KW_PROGRAMA ID DELIM_SEMICOLON [KW_VAR declaracao_variaveis] KW_INICIO bloco_comandos KW_FIM DELIM_DOT
void Parser::parsePrograma() {
    match(TokenType::KW_PROGRAMA, "Palavra-chave 'programa' esperada.");
    match(TokenType::ID, "Identificador do programa esperado.");
    match(TokenType::DELIM_SEMICOLON, "Ponto e virgula apos o nome do programa esperado.");

    if (currentToken.getType() == TokenType::KW_VAR) {
        parseDeclaracaoVariaveis();
    }

    match(TokenType::KW_INICIO, "Palavra-chave 'inicio' esperada.");
    parseBlocoComandos();
    match(TokenType::KW_FIM, "Palavra-chave 'fim' esperada.");
    match(TokenType::DELIM_DOT, "Ponto final apos o fim do programa esperado.");
}

// declaracao_variaveis ::= KW_VAR {lista_ids DELIM_COLON tipo DELIM_SEMICOLON}
void Parser::parseDeclaracaoVariaveis() {
    match(TokenType::KW_VAR, "Palavra-chave 'var' esperada.");
    while (currentToken.getType() == TokenType::ID) { // As declarações podem ser múltiplas
        parseListaIds();
        match(TokenType::DELIM_COLON, "Dois pontos apos a lista de identificadores esperado.");
        parseTipo();
        match(TokenType::DELIM_SEMICOLON, "Ponto e virgula apos a declaracao de tipo esperado.");
    }
}

// lista_ids ::= ID {DELIM_COMMA ID}
void Parser::parseListaIds() {
    match(TokenType::ID, "Identificador esperado na declaracao de variavel.");
    while (currentToken.getType() == TokenType::DELIM_COMMA) {
        match(TokenType::DELIM_COMMA, "Virgula esperada entre identificadores.");
        match(TokenType::ID, "Identificador esperado apos a virgula.");
    }
}

// tipo ::= KW_INTEIRO | KW_LOGICO
void Parser::parseTipo() {
    if (currentToken.getType() == TokenType::KW_INTEIRO) {
        match(TokenType::KW_INTEIRO, "Palavra-chave 'inteiro' esperada.");
    } else if (currentToken.getType() == TokenType::KW_LOGICO) {
        match(TokenType::KW_LOGICO, "Palavra-chave 'logico' esperada.");
    } else {
        syntaxError("Tipo de variavel (inteiro ou logico) esperado.");
    }
}

// bloco_comandos ::= {comando DELIM_SEMICOLON} KW_FIM DELIM_SEMICOLON (Ajustado para o fluxo)
// A função parseBlocoComandos é chamada *depois* do KW_INICIO e *antes* do KW_FIM.
// Ela deve parsear os comandos e garantir o ';' após cada um.
void Parser::parseBlocoComandos() {
    // Loop para zero ou mais comandos
    // Condições de parada: encontrar KW_FIM ou END_OF_FILE
    while (currentToken.getType() != TokenType::KW_FIM &&
           currentToken.getType() != TokenType::END_OF_FILE) {
        parseComando();
        match(TokenType::DELIM_SEMICOLON, "Ponto e virgula apos o comando esperado.");
    }
}


// comando ::= atribuicao | se | enquanto | ler | escrever
void Parser::parseComando() {
    if (currentToken.getType() == TokenType::ID) { // Pode ser atribuição ou futura chamada de sub-rotina
        parseAtribuicao();
    } else if (currentToken.getType() == TokenType::KW_SE) {
        parseSe();
    } else if (currentToken.getType() == TokenType::KW_ENQUANTO) {
        parseEnquanto();
    } else if (currentToken.getType() == TokenType::KW_LER) {
        parseLer();
    } else if (currentToken.getType() == TokenType::KW_ESCREVER) {
        parseEscrever();
    } else {
        syntaxError("Comando esperado (atribuicao, se, enquanto, ler, escrever).");
    }
}

// atribuicao ::= ID OP_ASSIGN expressao
void Parser::parseAtribuicao() {
    match(TokenType::ID, "Identificador esperado em atribuicao.");
    match(TokenType::OP_ASSIGN, "Operador de atribuicao (:=) esperado.");
    parseExpressao();
}

// se ::= KW_SE DELIM_LPAREN expressao DELIM_RPAREN KW_ENTAO KW_INICIO bloco_comandos KW_FIM [KW_SENAO KW_INICIO bloco_comandos KW_FIM]
// REMOVIDO: DELIM_SEMICOLON após KW_FIM, pois 'se' é um comando e o ';' é tratado por parseBlocoComandos chamador
void Parser::parseSe() {
    match(TokenType::KW_SE, "Palavra-chave 'se' esperada.");
    match(TokenType::DELIM_LPAREN, "Parentese esquerdo esperado apos 'se'.");
    parseExpressao();
    match(TokenType::DELIM_RPAREN, "Parentese direito esperado apos expressao.");
    match(TokenType::KW_ENTAO, "Palavra-chave 'entao' esperada.");
    match(TokenType::KW_INICIO, "Palavra-chave 'inicio' esperada para o bloco 'se'.");
    parseBlocoComandos();
    match(TokenType::KW_FIM, "Palavra-chave 'fim' esperada para o bloco 'se'.");
    // REMOVIDO: match(DELIM_SEMICOLON, "Ponto e virgula apos o bloco 'se' esperado.");

    if (currentToken.getType() == TokenType::KW_SENAO) {
        match(TokenType::KW_SENAO, "Palavra-chave 'senao' esperada.");
        match(TokenType::KW_INICIO, "Palavra-chave 'inicio' esperada para o bloco 'senao'.");
        parseBlocoComandos();
        match(TokenType::KW_FIM, "Palavra-chave 'fim' esperada para o bloco 'senao'.");
        // REMOVIDO: match(DELIM_SEMICOLON, "Ponto e virgula apos o bloco 'senao' esperado.");
    }
}

// enquanto ::= KW_ENQUANTO DELIM_LPAREN expressao DELIM_RPAREN KW_FACA KW_INICIO bloco_comandos KW_FIM
// REMOVIDO: DELIM_SEMICOLON após KW_FIM, pois 'enquanto' é um comando e o ';' é tratado por parseBlocoComandos chamador
void Parser::parseEnquanto() {
    match(TokenType::KW_ENQUANTO, "Palavra-chave 'enquanto' esperada.");
    match(TokenType::DELIM_LPAREN, "Parentese esquerdo esperado apos 'enquanto'.");
    parseExpressao();
    match(TokenType::DELIM_RPAREN, "Parentese direito esperado apos expressao.");
    match(TokenType::KW_FACA, "Palavra-chave 'faca' esperada.");
    match(TokenType::KW_INICIO, "Palavra-chave 'inicio' esperada para o bloco 'enquanto'.");
    parseBlocoComandos();
    match(TokenType::KW_FIM, "Palavra-chave 'fim' esperada para o bloco 'enquanto'.");
    // REMOVIDO: match(DELIM_SEMICOLON, "Ponto e virgula apos o bloco 'enquanto' esperado.");
}

// ler ::= KW_LER ID
void Parser::parseLer() {
    match(TokenType::KW_LER, "Palavra-chave 'ler' esperada.");
    match(TokenType::ID, "Identificador esperado em comando 'ler'.");
}

// escrever ::= KW_ESCREVER (STRING_LITERAL | ID | expressao) {DELIM_COMMA (STRING_LITERAL | ID | expressao)}
void Parser::parseEscrever() {
    match(TokenType::KW_ESCREVER, "Palavra-chave 'escrever' esperada.");
    // O parser deve tentar uma expressão primeiro, pois ID e NUMBER são fatores de expressão.
    // Se não for uma expressão, tenta STRING_LITERAL.
    // Isso evita um erro de 'esperado STRING_LITERAL' se for um ID ou NUMBER.
    if (currentToken.getType() == TokenType::STRING_LITERAL) {
        match(TokenType::STRING_LITERAL, "Literal de string ou expressao esperada.");
    } else {
        // Se não é STRING_LITERAL, assume que deve ser uma expressão.
        parseExpressao();
    }

    while (currentToken.getType() == TokenType::DELIM_COMMA) {
        match(TokenType::DELIM_COMMA, "Virgula esperada entre argumentos de 'escrever'.");
        if (currentToken.getType() == TokenType::STRING_LITERAL) {
            match(TokenType::STRING_LITERAL, "Literal de string ou expressao esperada.");
        } else {
            parseExpressao();
        }
    }
}

// expressao ::= expressao_logica {KW_OU expressao_logica}
void Parser::parseExpressao() {
    parseExpressaoLogica();
    while (currentToken.getType() == TokenType::KW_OU) {
        match(TokenType::KW_OU, "Operador 'ou' esperado.");
        parseExpressaoLogica();
    }
}

// expressao_logica ::= expressao_relacional {KW_E expressao_relacional}
void Parser::parseExpressaoLogica() {
    parseExpressaoRelacional();
    while (currentToken.getType() == TokenType::KW_E) {
        match(TokenType::KW_E, "Operador 'e' esperado.");
        parseExpressaoRelacional();
    }
}

// expressao_relacional ::= expressao_aditiva [relop expressao_aditiva]
void Parser::parseExpressaoRelacional() {
    parseExpressaoAditiva();
    if (currentToken.getType() == TokenType::OP_EQ || currentToken.getType() == TokenType::OP_NE ||
        currentToken.getType() == TokenType::OP_LT || currentToken.getType() == TokenType::OP_LE ||
        currentToken.getType() == TokenType::OP_GT || currentToken.getType() == TokenType::OP_GE) {
        // relop
        currentToken = lexer.getNextToken(); // Avança o operador relacional
        parseExpressaoAditiva();
    }
}

// expressao_aditiva ::= expressao_multiplicativa {addop expressao_multiplicativa}
void Parser::parseExpressaoAditiva() {
    parseExpressaoMultiplicativa();
    while (currentToken.getType() == TokenType::OP_PLUS || currentToken.getType() == TokenType::OP_MINUS) {
        currentToken = lexer.getNextToken(); // Avança o operador aditivo
        parseExpressaoMultiplicativa();
    }
}

// expressao_multiplicativa ::= fator {mulop fator}
void Parser::parseExpressaoMultiplicativa() {
    parseFator();
    while (currentToken.getType() == TokenType::OP_MULT || currentToken.getType() == TokenType::OP_DIV) {
        currentToken = lexer.getNextToken(); // Avança o operador multiplicativo
        parseFator();
    }
}

// fator ::= ID | NUMBER | KW_VERDADEIRO | KW_FALSO | DELIM_LPAREN expressao DELIM_RPAREN
void Parser::parseFator() {
    if (currentToken.getType() == TokenType::ID || currentToken.getType() == TokenType::NUMBER ||
        currentToken.getType() == TokenType::KW_VERDADEIRO || currentToken.getType() == TokenType::KW_FALSO) {
        currentToken = lexer.getNextToken(); // Consome o ID, NUMBER, TRUE ou FALSE
    } else if (currentToken.getType() == TokenType::DELIM_LPAREN) {
        match(TokenType::DELIM_LPAREN, "Parentese esquerdo esperado.");
        parseExpressao();
        match(TokenType::DELIM_RPAREN, "Parentese direito esperado.");
    } else {
        syntaxError("Fator esperado (identificador, numero, verdadeiro, falso ou expressao entre parenteses).");
    }
}