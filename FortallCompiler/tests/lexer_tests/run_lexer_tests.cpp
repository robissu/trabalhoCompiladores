// tests/lexer_tests/run_lexer_tests.cpp

#include <iostream>
#include <string>
#include <vector>
#include "../../src/Lexer/Lexer.h"
#include "../../src/Token/Token.h"
#include "../../src/Parser/Parser.h" // Inclui o cabeçalho do Parser

// Função para testar o Lexer
void testLexer(const std::string& code, const std::string& testName) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Testando (Lexer): " << testName << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    Lexer lexer(code);
    Token token = lexer.getNextToken(); // Inicializa 'token' diretamente com o primeiro token
    while (token.getType() != TokenType::END_OF_FILE) {
        std::cout << "Token: " << token.toString() << std::endl;
        token = lexer.getNextToken(); // Obtém o próximo token dentro do loop
    }
    // Exibe também o token de fim de arquivo para completude
    std::cout << "Token: " << token.toString() << std::endl;

    std::cout << "Analise Lexica concluida com sucesso para este codigo." << std::endl;
}

// Função para testar o Parser
void testParser(const std::string& code, const std::string& testName) {
    std::cout << "\n==========================================" << std::endl;
    std::cout << "Testando (Parser): " << testName << std::endl;
    std::cout << "------------------------------------------" << std::endl;

    Lexer lexer(code);
    Parser parser(lexer);

    try {
        parser.parse();
        std::cout << "Analise Sintatica concluida com sucesso!" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Erro de Parsing: " << e.what() << std::endl;
    }
}

int main() {
    std::cout << "--- Testando o Analisador Lexico e Sintatico FORTALL ---" << std::endl;

    // --- Exemplo de Código 1: Código Válido (para Lexer e Parser, AGORA CONFORME A GRAMATICA) ---
    std::string test_code_1 = R"(
        programa Exemplo;
        var
            contador : inteiro;
            nome : logico;
        inicio
            contador := 10 + 5;
            enquanto (contador > 0) faca
                inicio // Adicionado para conformidade com a gramática 'bloco_comandos'
                    escrever 'Contador: ', contador;
                    contador := contador - 1;
                fim; // Semicolon após o fim do bloco interno
            se (nome = falso) entao
                inicio // Adicionado para conformidade com a gramática 'bloco_comandos'
                    ler nome;
                fim; // Semicolon após o fim do bloco interno
            senao
                inicio // Adicionado para conformidade com a gramática 'bloco_comandos'
                    escrever 'Nome nao definido', '\n', 'bye';
                fim; // Semicolon após o fim do bloco interno
            fim; // Semicolon após o fim do bloco 'inicio' principal
        fim.
    )";
    testLexer(test_code_1, "Exemplo 1 (Codigo Valido)");
    testParser(test_code_1, "Exemplo 1 (Codigo Valido)");

    // --- Exemplo de Código 2: Erro Léxico Intencional ---
    std::string test_code_2 = R"(
        programa ErroLexico;
        var
            x : inteiro;
        inicio
            x := 10 + @5; // Caractere '@' não reconhecido
            // Comentario invalido { sem fechar
            escrever 'e', ',', ',', '┬ú', 'i', 'lidos';
        fim.
    )";
    testLexer(test_code_2, "Exemplo 2 (Erro Lexico)");
    // testParser(test_code_2, "Exemplo 2 (Erro Lexico)"); // Parser não deve rodar em código com erro léxico.

    // --- Exemplo de Código 3: Erro Sintático (Falta de ';') ---
    std::string test_code_3 = R"(
        programa TesteSintatico;
        var
            a : inteiro // Falta ';' após a declaração de 'a'
        inicio
            a := 10;
        fim.
    )";
    testParser(test_code_3, "Exemplo 3 (Erro Sintatico - Falta ;)");

    // --- Exemplo de Código 4: Expressões e Comandos Complexos ---
    std::string test_code_4 = R"(
        programa ExemploExpressao;
        var
            a, b : inteiro;
            cond : logico;
        inicio
            a := (10 + 5) * 2 - 3;
            b := a / 2;
            cond := (a > b) e (verdadeiro ou falso); // 'e' e 'ou' agora devem ser KW_E e KW_OU
            se (cond) entao
                inicio // Bloco de comando interno
                    escrever 'Condicao verdadeira';
                fim; // Semicolon após o fim do bloco interno
            fim; // Semicolon após o fim do bloco 'inicio' principal
        fim.
    )";
    testParser(test_code_4, "Exemplo 4 (Expressoes e Comandos)");

    return 0;
}