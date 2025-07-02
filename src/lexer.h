
#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>
#include <unordered_map>

class Lexer {
private:
    std::string input;
    size_t position;
    int line;
    int column;
    std::unordered_map<std::string, TokenType> keywords;
    
    void initKeywords();
    char currentChar();
    char peek();
    void advance();
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readString();
    Token readIdentifier();
    
public:
    Lexer(const std::string& input);
    Token nextToken();
    bool hasError() const { return false; }
};

#endif
