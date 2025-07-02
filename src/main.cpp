
#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "interpreter.h"
#include "symbol_table.h"

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void showHelp()
{
    std::cout << "\n=== COMPILADOR/INTERPRETADOR FORTALL ===" << std::endl;
    std::cout << "Uso: fortall <arquivo.fort>" << std::endl;
    std::cout << "Comandos disponiveis:" << std::endl;
    std::cout << "  help    - Mostra esta ajuda" << std::endl;
    std::cout << "  exit    - Sair do programa" << std::endl;
    std::cout << "  test    - Executar todos os testes" << std::endl;
    std::cout << "\nExemplo: fortall programa.fort" << std::endl;
}

bool compileAndRun(const std::string &filename)
{
    std::cout << "Compilando arquivo: " << filename << std::endl;

    std::string source = readFile(filename);
    if (source.empty())
    {
        std::cout << "Erro: Nao foi possível ler o arquivo '" << filename << "'" << std::endl;
        return false;
    }

    // analise lexica
    std::cout << "Executando analise lexica..." << std::endl;
    Lexer lexer(source);

    // analise sintatica
    std::cout << "Executando analise sintatica..." << std::endl;
    Parser parser(lexer);
    auto ast = parser.parse();

    if (parser.hasError())
    {
        std::cout << parser.getError() << std::endl;
        return false;
    }

    if (!ast)
    {
        std::cout << "Erro: Falha na analise sintatica" << std::endl;
        return false;
    }

    // analise semantica
    std::cout << "Executando analise semantica..." << std::endl;
    SymbolTable symbolTable;
    SemanticAnalyzer semantic(symbolTable);

    if (!semantic.analyze(ast))
    {
        std::cout << semantic.getError() << std::endl;
        return false;
    }

    // Execução
    std::cout << "Compilacao bem-sucedida! Executando programa..." << std::endl;
    std::cout << "===========================================" << std::endl;

    Interpreter interpreter(symbolTable);
    if (!interpreter.execute(ast))
    {
        std::cout << std::endl
                  << "===========================================" << std::endl;
        std::cout << interpreter.getError() << std::endl;
        return false;
    }

    std::cout << "===========================================" << std::endl;
    std::cout << "Programa executado com sucesso!" << std::endl;
    return true;
}

void runTests() {
    std::cout << "\n=== EXECUTANDO TESTES ===" << std::endl;
    
    for (int i = 1; i <= 6; i++) { //  '5' para o número total de seus testes
        
        std::string filename = "tests/test" + std::to_string(i) + ".fort"; // <--caminho da pasta
        
        std::cout << "\n--- TESTE " << i << " ---" << std::endl;
        
        if (compileAndRun(filename)) {
            std::cout << "TESTE " << i << ": PASSOU" << std::endl;
        } else {
            std::cout << "TESTE " << i << ": FALHOU" << std::endl;
        }
    }
    
    std::cout << "\n=== TESTES CONCLUIDOS ===" << std::endl;
}

int main(int argc, char *argv[])
{

    std::cout << "=== COMPILADOR/INTERPRETADOR FORTALL ===" << std::endl;

    std::cout << "Versao 1.0 - Desenvolvido em C++" << std::endl;

    if (argc == 2)
    {

        std::string arg = argv[1];

        if (arg == "help")
        {

            showHelp();

            return 0;
        }
        else if (arg == "test")
        {

            runTests();

            return 0;
        }
        else
        {

            compileAndRun(arg);

            return 0;
        }
    }

    // Modo interativo

    showHelp();

    std::string input;

    while (true)
    {

        std::cout << "\nfortall> ";

        std::getline(std::cin, input);

        if (input == "exit")
        {

            break;
        }
        else if (input == "help")
        {

            showHelp();
        }
        else if (input == "test")
        {

            runTests();
        }
        else if (!input.empty())
        {

            compileAndRun(input);
        }
    }

    std::cout << "Obrigado por usar o Compilador Fortall!" << std::endl;

    return 0;
}
