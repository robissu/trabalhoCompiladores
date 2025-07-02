
#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include "ast.h"
#include <memory>

class Parser {
private:
    Lexer& lexer;
    Token currentToken;
    std::string errorMessage;
    
    void advance();
    bool match(TokenType expected);
    bool expect(TokenType expected);
    void error(const std::string& message);
    
    ASTNodePtr parsePrograma();
    ASTNodePtr parseDeclaracoes();
    ASTNodePtr parseDeclaracao();
    ASTNodePtr parseListaVar();
    ASTNodePtr parseTipo();
    ASTNodePtr parseListaComandos();
    ASTNodePtr parseComando();
    ASTNodePtr parseAtribuicao();
    ASTNodePtr parseSe();
    ASTNodePtr parseEnquanto();
    ASTNodePtr parseLer();
    ASTNodePtr parseEscrever();
    ASTNodePtr parseExpressao();
    ASTNodePtr parseExpressaoLogica();
    ASTNodePtr parseExpressaoRelacional();
    ASTNodePtr parseExpressaoAritmetica();
    ASTNodePtr parseTermo();
    ASTNodePtr parseFator();
    
public:
    Parser(Lexer& lexer);
    ASTNodePtr parse();
    bool hasError() const { return !errorMessage.empty(); }
    const std::string& getError() const { return errorMessage; }
};

#endif
