#include "lexer.h"
#include <cctype>
#include <iostream>

std::string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TokenType::PROGRAMA:
        return "PROGRAMA";
    case TokenType::INICIO:
        return "INICIO";
    case TokenType::FIM:
        return "FIM";
    case TokenType::VAR:
        return "VAR";
    case TokenType::INTEIRO:
        return "INTEIRO";
    case TokenType::LOGICO:
        return "LOGICO";
    case TokenType::SE:
        return "SE";
    case TokenType::ENTAO:
        return "ENTAO";
    case TokenType::SENAO:
        return "SENAO";
    case TokenType::ENQUANTO:
        return "ENQUANTO";
    case TokenType::FACA:
        return "FACA";
    case TokenType::LER:
        return "LER";
    case TokenType::ESCREVER:
        return "ESCREVER";
    case TokenType::VERDADEIRO:
        return "VERDADEIRO";
    case TokenType::FALSO:
        return "FALSO";
    case TokenType::FIM_ENQUANTO:
        return "FIM_ENQUANTO";
    case TokenType::IDENTIFICADOR:
        return "IDENTIFICADOR";
    case TokenType::NUMERO:
        return "NUMERO";
    case TokenType::STRING:
        return "STRING";
    case TokenType::MAIS:
        return "MAIS";
    case TokenType::MENOS:
        return "MENOS";
    case TokenType::MULTIPLICACAO:
        return "MULTIPLICACAO";
    case TokenType::DIVISAO:
        return "DIVISAO";
    case TokenType::IGUAL:
        return "IGUAL";
    case TokenType::DIFERENTE:
        return "DIFERENTE";
    case TokenType::MENOR:
        return "MENOR";
    case TokenType::MENOR_IGUAL:
        return "MENOR_IGUAL";
    case TokenType::MAIOR:
        return "MAIOR";
    case TokenType::MAIOR_IGUAL:
        return "MAIOR_IGUAL";
    case TokenType::ATRIBUICAO:
        return "ATRIBUICAO";
    case TokenType::PONTO_VIRGULA:
        return "PONTO_VIRGULA";
    case TokenType::PONTO:
        return "PONTO";
    case TokenType::VIRGULA:
        return "VIRGULA";
    case TokenType::DOIS_PONTOS:
        return "DOIS_PONTOS";
    case TokenType::PARENTESE_ESQ:
        return "PARENTESE_ESQ";
    case TokenType::PARENTESE_DIR:
        return "PARENTESE_DIR";
    case TokenType::COLCHETE_ESQ:
        return "COLCHETE_ESQ";
    case TokenType::COLCHETE_DIR:
        return "COLCHETE_DIR";
    case TokenType::FIM_ARQUIVO:
        return "FIM_ARQUIVO";
    case TokenType::ERRO:
        return "ERRO";
    case TokenType::FIM_SE:
        return "FIM_SE";
    case TokenType::COMENTARIO:
        return "COMENTARIO";
    default:
        return "DESCONHECIDO";
    }
}

Lexer::Lexer(const std::string &input) : input(input), position(0), line(1), column(1)
{
    initKeywords();
}

void Lexer::initKeywords()
{
    keywords["programa"] = TokenType::PROGRAMA;
    keywords["inicio"] = TokenType::INICIO;
    keywords["fim"] = TokenType::FIM;
    keywords["var"] = TokenType::VAR;
    keywords["inteiro"] = TokenType::INTEIRO;
    keywords["logico"] = TokenType::LOGICO;
    keywords["se"] = TokenType::SE;
    keywords["entao"] = TokenType::ENTAO;
    keywords["senao"] = TokenType::SENAO;
    keywords["enquanto"] = TokenType::ENQUANTO;
    keywords["faca"] = TokenType::FACA;
    keywords["ler"] = TokenType::LER;
    keywords["escrever"] = TokenType::ESCREVER;
    keywords["verdadeiro"] = TokenType::VERDADEIRO;
    keywords["falso"] = TokenType::FALSO;
    keywords["fim_enquanto"] = TokenType::FIM_ENQUANTO;
    keywords["fim_se"] = TokenType::FIM_SE;
}

char Lexer::currentChar()
{
    if (position >= input.length())
    {
        return '\0';
    }
    return input[position];
}

char Lexer::peek()
{
    if (position + 1 >= input.length())
    {
        return '\0';
    }
    return input[position + 1];
}

void Lexer::advance()
{
    if (position < input.length() && input[position] == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }
    position++;
}

void Lexer::skipWhitespace()
{
    while (currentChar() != '\0' && std::isspace(currentChar()))
    {
        advance();
    }
}

void Lexer::skipComment()
{
    if (currentChar() == '{')
    {
        advance(); // Consome '{'
        while (currentChar() != '\0' && currentChar() != '}')
        {
            advance();
        }
        if (currentChar() == '}')
        {
            advance(); // Consome '}'
        }
        else
        {
            // Se o comentário nao for fechado, você pode querer reportar um erro aqui
            // Por enquanto, apenas avança até o fim do arquivo ou do bloco de texto
        }
    }
}

Token Lexer::readNumber()
{
    std::string number;
    int startColumn = column;

    while (currentChar() != '\0' && std::isdigit(currentChar()))
    {
        number += currentChar();
        advance();
    }

    return Token(TokenType::NUMERO, number, line, startColumn);
}

Token Lexer::readString()
{
    std::string str;
    int startColumn = column;

    advance(); // Skip opening quote

    // Você tinha 'String' no seu Token.h mas aqui está lendo com aspas simples.
    // Presumo que 'STRING' seja para 'string literals' delimitadas por aspas simples, como no Pascal.
    while (currentChar() != '\0' && currentChar() != '\'')
    {
        // Lógica para caracteres de escape (se Fortall suportar, como '\n', '\t', etc.)
        // CUIDADO: código atual parece suportar '\n' e '\t' mas apenas quando '\' é o char.
        // No Fortall (Pascal-like), aspas dentro de strings são geralmente duplicadas ('It''s').

        str += currentChar();
        advance();
    }

    if (currentChar() == '\'')
    {
        advance(); 
    }
    else
    {
        
        return Token(TokenType::ERRO, str, line, startColumn);
    }

    return Token(TokenType::STRING, str, line, startColumn);
}

Token Lexer::readIdentifier()
{
    std::string identifier;
    int startColumn = column;

    while (currentChar() != '\0' && (std::isalnum(currentChar()) || currentChar() == '_'))
    {
        identifier += std::tolower(currentChar()); // Converte para minúsculas para comparação de palavras-chave
        advance();
    }

    TokenType type = TokenType::IDENTIFICADOR;
    auto it = keywords.find(identifier); // Procura no mapa de palavras-chave
    if (it != keywords.end())
    {
        type = it->second; // Se encontrado, é uma palavra-chave
    }

    return Token(type, identifier, line, startColumn);
}

Token Lexer::nextToken() {
    // Outer loop to continuously skip whitespace and comments
    // until a significant character or EOF is found.
    while (true) {
        skipWhitespace();
        
        // Check if we hit EOF after skipping whitespace
        if (currentChar() == '\0') {
            return Token(TokenType::FIM_ARQUIVO, "", line, column);
        }

        
        size_t oldPosition = position; 
        skipComment();

       
        if (position != oldPosition) {
            continue; 
        }
        
        // If we reach here, it means:
        // 1. All leading whitespace has been skipped.
        // 2. No comment was found (or if it was, we looped back and now we're past it).
        // So, the current character must be the start of a real token or EOF.
        break; 
    }

   
    int currentLine = line;
    int currentColumn = column;
    
  

    if (std::isalpha(currentChar()) || currentChar() == '_') {
        return readIdentifier();
    }
    
    if (std::isdigit(currentChar())) {
        return readNumber();
    }
    
    if (currentChar() == '\'') {
        return readString();
    }
    
    switch (currentChar()) {
        case '+':
            advance();
            return Token(TokenType::MAIS, "+", currentLine, currentColumn);
        case '-':
            advance();
            return Token(TokenType::MENOS, "-", currentLine, currentColumn);
        case '*':
            advance();
            return Token(TokenType::MULTIPLICACAO, "*", currentLine, currentColumn);
        case '/':
            advance();
            return Token(TokenType::DIVISAO, "/", currentLine, currentColumn);
        case '=':
            advance();
            return Token(TokenType::IGUAL, "=", currentLine, currentColumn);
        case '<':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::MENOR_IGUAL, "<=", currentLine, currentColumn);
            } else if (currentChar() == '>') {
                advance();
                return Token(TokenType::DIFERENTE, "<>", currentLine, currentColumn);
            }
            return Token(TokenType::MENOR, "<", currentLine, currentColumn);
        case '>':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::MAIOR_IGUAL, ">=", currentLine, currentColumn);
            }
            return Token(TokenType::MAIOR, ">", currentLine, currentColumn);
        case ':':
            advance();
            if (currentChar() == '=') {
                advance();
                return Token(TokenType::ATRIBUICAO, ":=", currentLine, currentColumn);
            }
            return Token(TokenType::DOIS_PONTOS, ":", currentLine, currentColumn);
        case ';':
            advance();
            return Token(TokenType::PONTO_VIRGULA, ";", currentLine, currentColumn);
        case '.':
            advance();
            return Token(TokenType::PONTO, ".", currentLine, currentColumn);
        case ',':
            advance();
            return Token(TokenType::VIRGULA, ",", currentLine, currentColumn);
        case '(':
            advance();
            return Token(TokenType::PARENTESE_ESQ, "(", currentLine, currentColumn);
        case ')':
            advance();
            return Token(TokenType::PARENTESE_DIR, ")", currentLine, currentColumn);
        case '[': 
            advance();
            return Token(TokenType::COLCHETE_ESQ, "[", currentLine, currentColumn);
        case ']': 
            advance();
            return Token(TokenType::COLCHETE_DIR, "]", currentLine, currentColumn);
        default:
            std::string errorChar(1, currentChar());
            advance();
            return Token(TokenType::ERRO, errorChar, currentLine, currentColumn);
    }
}
