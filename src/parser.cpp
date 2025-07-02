
#include "parser.h"
#include <iostream>

Parser::Parser(Lexer &lexer) : lexer(lexer)
{
    currentToken = lexer.nextToken();
}

void Parser::advance()
{
    currentToken = lexer.nextToken();
    while (currentToken.type == TokenType::COMENTARIO) {
        currentToken = lexer.nextToken();
    }
}
bool Parser::match(TokenType expected)
{
    return currentToken.type == expected;
}

bool Parser::expect(TokenType expected)
{
    if (match(expected))
    {
        advance();
        return true;
    }
    return false;
}

void Parser::error(const std::string &message)
{
    errorMessage = "Erro sintatico na linha " + std::to_string(currentToken.line) +
                   ", coluna " + std::to_string(currentToken.column) + ": " + message;
}

ASTNodePtr Parser::parse()
{
    return parsePrograma();
}

ASTNodePtr Parser::parsePrograma()
{
    auto node = std::make_shared<ASTNode>(NodeType::PROGRAMA);

    if (!expect(TokenType::PROGRAMA))
    {
        error("Esperado 'programa'");
        return nullptr;
    }

    if (!match(TokenType::IDENTIFICADOR))
    {
        error("Esperado nome do programa");
        return nullptr;
    }

    node->token = currentToken;
    advance();

    if (!expect(TokenType::PONTO_VIRGULA))
    {
        error("Esperado ';' apos nome do programa");
        return nullptr;
    }

    // Declarações opcionais
    if (match(TokenType::VAR))
    {
        auto decl = parseDeclaracoes();
        if (decl)
            node->addChild(decl);
    }

    if (!expect(TokenType::INICIO))
    {
        error("Esperado 'inicio'");
        return nullptr;
    }

    auto comandos = parseListaComandos();
    if (comandos)
        node->addChild(comandos);

    if (!expect(TokenType::FIM))
    {
        error("Esperado 'fim'");
        return nullptr;
    }

    if (!expect(TokenType::PONTO))
    {
        error("Esperado '.' no final do programa");
        return nullptr;
    }

    return node;
}

ASTNodePtr Parser::parseDeclaracoes()
{
    auto node = std::make_shared<ASTNode>(NodeType::DECLARACAO);

    if (!expect(TokenType::VAR))
    {
        return nullptr;
    }

    do
    {
        auto decl = parseDeclaracao();
        if (decl)
            node->addChild(decl);

        if (!expect(TokenType::PONTO_VIRGULA))
        {
            error("Esperado ';' apos declaração");
            return nullptr;
        }
    } while (match(TokenType::IDENTIFICADOR));

    return node;
}

ASTNodePtr Parser::parseDeclaracao()
{
    auto node = std::make_shared<ASTNode>(NodeType::DECLARACAO);

    auto listaVar = parseListaVar();
    if (listaVar)
        node->addChild(listaVar);

    if (!expect(TokenType::DOIS_PONTOS))
    {
        error("Esperado ':' na declaração");
        return nullptr;
    }

    auto tipo = parseTipo();
    if (tipo)
        node->addChild(tipo);

    return node;
}

ASTNodePtr Parser::parseListaVar()
{
    auto node = std::make_shared<ASTNode>(NodeType::LISTA_VAR);

    if (!match(TokenType::IDENTIFICADOR))
    {
        error("Esperado identificador");
        return nullptr;
    }

    auto id = std::make_shared<ASTNode>(NodeType::IDENTIFICADOR, currentToken);
    node->addChild(id);
    advance();

    while (match(TokenType::VIRGULA))
    {
        advance();
        if (!match(TokenType::IDENTIFICADOR))
        {
            error("Esperado identificador apos ','");
            return nullptr;
        }
        auto nextId = std::make_shared<ASTNode>(NodeType::IDENTIFICADOR, currentToken);
        node->addChild(nextId);
        advance();
    }

    return node;
}

ASTNodePtr Parser::parseTipo()
{
    auto node = std::make_shared<ASTNode>(NodeType::TIPO);

    if (match(TokenType::INTEIRO) || match(TokenType::LOGICO))
    {
        node->token = currentToken;
        advance();
        return node;
    }

    error("Esperado tipo 'inteiro' ou 'logico'");
    return nullptr;
}

ASTNodePtr Parser::parseListaComandos()
{
    auto node = std::make_shared<ASTNode>(NodeType::LISTA_COMANDOS);

    // O loop continua enquanto o token atual NÃO for um dos delimitadores de fim de bloco
    while (!match(TokenType::FIM) &&            // FIM do programa
           !match(TokenType::FIM_ARQUIVO) &&    // FIM do arquivo
           !match(TokenType::SENAO) &&          // SENAO do bloco SE
           !match(TokenType::FIM_SE) &&         // FIM_SE do bloco SE
           !match(TokenType::FIM_ENQUANTO))     // FIM_ENQUANTO do bloco ENQUANTO
    {
        auto comando = parseComando();
        if (comando)
        {
            node->addChild(comando);

            // === TRATAMENTO DE PONTO E VÍRGULA ===
            // Comandos como SE e ENQUANTO já gerenciam seu próprio fim de bloco
            // e não exigem um ponto e vírgula após o seu término.
            // Outros comandos (atribuição, ler, escrever) geralmente exigem.
            if (comando->type != NodeType::SE && comando->type != NodeType::ENQUANTO) {
                if (!expect(TokenType::PONTO_VIRGULA)) {
                    // Se o ponto e vírgula estiver faltando, mas o token atual for um
                    // delimitador de bloco (que não exige ';' precedente), não é um erro.
                    if (currentToken.type != TokenType::FIM_SE &&
                        currentToken.type != TokenType::SENAO &&
                        currentToken.type != TokenType::FIM_ENQUANTO &&
                        currentToken.type != TokenType::FIM &&
                        currentToken.type != TokenType::FIM_ARQUIVO)
                    {
                        error("Esperado ';' apos comando.");
                        return nullptr;
                    }
                }
            }
            // Para comandos SE ou ENQUANTO, nenhum ';' é esperado aqui.
            // O loop simplesmente continuará para o próximo comando ou terminará
            // se um delimitador global (como FIM) for encontrado.
        }
        else
        {
            // Se parseComando retornou nullptr (sem um comando válido),
            // e não estamos em um token de término de bloco esperado (ou já há um erro),
            // então é um novo erro.
            if (!hasError() &&
                !match(TokenType::FIM) && !match(TokenType::FIM_ARQUIVO) &&
                !match(TokenType::SENAO) && !match(TokenType::FIM_SE) && !match(TokenType::FIM_ENQUANTO))
            {
                error("Comando inesperado ou faltou ';'.");
                return nullptr; // Propaga o erro
            }
            break; // Se é um token de término de bloco ou já há erro, sai do loop.
                   // A função pai (parseSe, parseEnquanto, parsePrograma) irá lidar com o token.
        }
    }
    return node;
}

ASTNodePtr Parser::parseComando()
{
    if (match(TokenType::IDENTIFICADOR))
    {
        return parseAtribuicao();
    }
    else if (match(TokenType::SE))
    {
        return parseSe();
    }
    else if (match(TokenType::ENQUANTO))
    {
        return parseEnquanto();
    }
    else if (match(TokenType::LER))
    {
        return parseLer();
    }
    else if (match(TokenType::ESCREVER))
    {
        return parseEscrever();
    }

    return nullptr;
}

ASTNodePtr Parser::parseAtribuicao()
{
    auto node = std::make_shared<ASTNode>(NodeType::ATRIBUICAO);

    if (!match(TokenType::IDENTIFICADOR))
    {
        error("Esperado identificador na atribuicao");
        return nullptr;
    }

    auto id = std::make_shared<ASTNode>(NodeType::IDENTIFICADOR, currentToken);
    node->addChild(id);
    advance();

    if (!expect(TokenType::ATRIBUICAO))
    {
        error("Esperado ':=' na atribuicao");
        return nullptr;
    }

    auto expr = parseExpressao();
    if (expr)
        node->addChild(expr);

    return node;
}

ASTNodePtr Parser::parseSe()
{
    auto node = std::make_shared<ASTNode>(NodeType::SE);

    // 1. Consome 'se'
    if (!expect(TokenType::SE)) {
        return nullptr;
    }

    // 2. Lida com os parênteses opcionais da condição
    bool hasParentheses = false;
    if (match(TokenType::PARENTESE_ESQ)) {
        hasParentheses = true;
        advance(); // Consome '('
    }

    // 3. Analisa a expressão condicional
    auto condicao = parseExpressao();
    if (!condicao) {
        return nullptr;
    }
    node->addChild(condicao);

    // 4. Consome ')' se houver parênteses
    if (hasParentheses) {
        if (!expect(TokenType::PARENTESE_DIR)) {
            error("Esperado ')' apos a condicao do 'se'.");
            return nullptr;
        }
    }

    // 5. Consome 'entao'
    if (!expect(TokenType::ENTAO)) {
        error("Esperado 'entao' apos condicao.");
        return nullptr;
    }

    // 6. Espera a LISTA DE COMANDOS para o bloco 'ENTAO'
    auto blocoEntao = parseListaComandos();
    if (!blocoEntao) {
        return nullptr;
    }
    node->addChild(blocoEntao);

    // --- Trata o SENAO e o FIM_SE UNIFICADAMENTE ---
    // O FIM_SE final só é esperado uma vez para toda a estrutura SE-SENAO.

    if (match(TokenType::SENAO)) { // Se há uma cláusula 'senao'
        advance(); // Consome 'senao'
        
        // 7. Espera a LISTA DE COMANDOS para o bloco 'SENAO'
        auto blocoSenao = parseListaComandos();
        if (!blocoSenao) {
            return nullptr;
        }
        node->addChild(blocoSenao);

        // 8. O FIM_SE para toda a estrutura SE-SENAO, APÓS o bloco SENAO
        if (!expect(TokenType::FIM_SE)) {
            error("Esperado 'fim_se' apos o bloco 'senao'.");
            return nullptr;
        }
    } else { // Se NÃO HÁ cláusula 'senao'
        // 9. O FIM_SE para o SE sem SENAO, APÓS o bloco ENTAO
        if (!expect(TokenType::FIM_SE)) {
            error("Esperado 'fim_se' apos o bloco 'entao'.");
            return nullptr;
        }
    }

    return node;
}
ASTNodePtr Parser::parseEnquanto()
{
    auto whileNode = std::make_shared<ASTNode>(NodeType::ENQUANTO, currentToken);

    if (!expect(TokenType::ENQUANTO)) // Consome 'enquanto'
    {
        return nullptr;
    }


    // Verifica se há um parêntese esquerdo (ABRE_PARENTESES)
    bool hasParentheses = false;
    if (match(TokenType::PARENTESE_ESQ)) { // Verifica se o próximo token é '('
        hasParentheses = true;
        advance(); // Consome o '('
    }

    auto condition = parseExpressao(); // Parsers a expressão (que pode ter outros parênteses internos)
    if (!condition)
    {
        error("Condição esperada para o comando 'enquanto'.");
        return nullptr;
    }
    whileNode->addChild(condition);

    // Se encontramos um '(' antes da expressão, então esperamos um ')' depois dela
    if (hasParentheses) {
        if (!expect(TokenType::PARENTESE_DIR)) { // Consome o ')'
            error("Esperado ')' apos a condição do 'enquanto'.");
            return nullptr;
        }
    }


    if (!expect(TokenType::FACA)) // Consome 'faca'
    {
        error("Esperado 'faca' apos a condição do 'enquanto'.");
        return nullptr;
    }

    auto bodyCommands = std::make_shared<ASTNode>(NodeType::LISTA_COMANDOS);

    // Loop para ler comandos até encontrar 'fim_enquanto'
    while (!match(TokenType::FIM_ENQUANTO) && !hasError() && !match(TokenType::FIM_ARQUIVO))
    {
        auto command = parseComando();
        if (command)
        {
            bodyCommands->addChild(command);
        }
        else
        {
            if (!hasError())
            {
                error("Comando inesperado no bloco 'enquanto'.");
            }
            return nullptr;
        }

        // Correção no ';': um comando simples deve ter ';'. Um bloco de chaves não.
        // Se a regra é "qualquer comando dentro do laço deve ter ';'",
        // então seu parseComando() precisa ser capaz de diferenciar um comando simples de um bloco.
        // Assumindo que parseComando() já retorna um comando simples ou um bloco completo.
        // Se `parseComando` nunca retorna um bloco, então o ';' é sempre obrigatório.

        // Esta parte da sua lógica para o ';' já está bem próxima do que precisa,
        // mas é bom confirmar se parseComando() pode retornar um bloco {}
        // Se ele pode retornar um bloco, a lógica do ';' precisa ser mais sofisticada.
        // Por enquanto, vamos manter a sua lógica do ';', que já parece tratar a ausência
        // antes de FIM_ENQUANTO como uma exceção.
        if (!expect(TokenType::PONTO_VIRGULA))
        {
            if (currentToken.type != TokenType::FIM_ENQUANTO && currentToken.type != TokenType::FIM_ARQUIVO)
            {
                error("Esperado ';' apos comando dentro do 'enquanto'.");
                return nullptr;
            }
        }
    }

    if (!expect(TokenType::FIM_ENQUANTO)) // Consome 'fim_enquanto'
    {
        error("Esperado 'fim_enquanto' para fechar o bloco 'enquanto'.");
        return nullptr;
    }

    whileNode->addChild(bodyCommands);

    return whileNode;
}

ASTNodePtr Parser::parseLer()
{
    auto node = std::make_shared<ASTNode>(NodeType::LER);

    if (!expect(TokenType::LER))
    {
        return nullptr;
    }

    bool temParenteses = false;
    if (match(TokenType::PARENTESE_ESQ))
    {
        temParenteses = true;
        advance();
    }

    if (!match(TokenType::IDENTIFICADOR))
    {
        error("Esperado identificador em 'ler'");
        return nullptr;
    }

    auto id = std::make_shared<ASTNode>(NodeType::IDENTIFICADOR, currentToken);
    node->addChild(id);
    advance();

    while (match(TokenType::VIRGULA))
    {
        advance();
        if (!match(TokenType::IDENTIFICADOR))
        {
            error("Esperado identificador apos ','");
            return nullptr;
        }
        auto nextId = std::make_shared<ASTNode>(NodeType::IDENTIFICADOR, currentToken);
        node->addChild(nextId);
        advance();
    }

    if (temParenteses && !expect(TokenType::PARENTESE_DIR))
    {
        error("Esperado ')' em 'ler'");
        return nullptr;
    }

    return node;
}

ASTNodePtr Parser::parseEscrever()
{
    auto node = std::make_shared<ASTNode>(NodeType::ESCREVER);

    if (!expect(TokenType::ESCREVER))
    {
        return nullptr;
    }

    bool temParenteses = false;
    if (match(TokenType::PARENTESE_ESQ))
    {
        temParenteses = true;
        advance();
    }

    auto expr = parseExpressao();
    if (expr)
        node->addChild(expr);

    while (match(TokenType::VIRGULA))
    {
        advance();
        auto nextExpr = parseExpressao();
        if (nextExpr)
            node->addChild(nextExpr);
    }

    if (temParenteses && !expect(TokenType::PARENTESE_DIR))
    {
        error("Esperado ')' em 'escrever'");
        return nullptr;
    }

    return node;
}

ASTNodePtr Parser::parseExpressao()
{
    return parseExpressaoLogica();
}

ASTNodePtr Parser::parseExpressaoLogica()
{
    return parseExpressaoRelacional();
}

ASTNodePtr Parser::parseExpressaoRelacional()
{
    auto left = parseExpressaoAritmetica();

    while (match(TokenType::IGUAL) || match(TokenType::DIFERENTE) ||
           match(TokenType::MENOR) || match(TokenType::MENOR_IGUAL) ||
           match(TokenType::MAIOR) || match(TokenType::MAIOR_IGUAL))
    {

        auto node = std::make_shared<ASTNode>(NodeType::BINARIO, currentToken);
        advance();

        auto right = parseExpressaoAritmetica();

        node->addChild(left);
        node->addChild(right);
        left = node;
    }

    return left;
}

ASTNodePtr Parser::parseExpressaoAritmetica()
{
    auto left = parseTermo();

    while (match(TokenType::MAIS) || match(TokenType::MENOS))
    {
        auto node = std::make_shared<ASTNode>(NodeType::BINARIO, currentToken);
        advance();

        auto right = parseTermo();

        node->addChild(left);
        node->addChild(right);
        left = node;
    }

    return left;
}

ASTNodePtr Parser::parseTermo()
{
    auto left = parseFator();

    while (match(TokenType::MULTIPLICACAO) || match(TokenType::DIVISAO))
    {
        auto node = std::make_shared<ASTNode>(NodeType::BINARIO, currentToken);
        advance();

        auto right = parseFator();

        node->addChild(left);
        node->addChild(right);
        left = node;
    }

    return left;
}

ASTNodePtr Parser::parseFator()
{
    if (match(TokenType::NUMERO))
    {
        auto node = std::make_shared<ASTNode>(NodeType::NUMERO, currentToken);
        advance();
        return node;
    }

    if (match(TokenType::STRING))
    {
        auto node = std::make_shared<ASTNode>(NodeType::STRING_LITERAL, currentToken);
        advance();
        return node;
    }

    if (match(TokenType::IDENTIFICADOR))
    {
        auto node = std::make_shared<ASTNode>(NodeType::IDENTIFICADOR, currentToken);
        advance();
        return node;
    }

    if (match(TokenType::VERDADEIRO) || match(TokenType::FALSO))
    {
        auto node = std::make_shared<ASTNode>(NodeType::LITERAL, currentToken);
        advance();
        return node;
    }

    if (match(TokenType::PARENTESE_ESQ))
    {
        advance();
        auto expr = parseExpressao();
        if (!expect(TokenType::PARENTESE_DIR))
        {
            error("Esperado ')' apos expressão");
        }
        return expr;
    }

    if (match(TokenType::MENOS))
    {
        auto node = std::make_shared<ASTNode>(NodeType::UNARIO, currentToken);
        advance();
        auto operand = parseFator();
        if (operand)
            node->addChild(operand);
        return node;
    }

    error("Esperado número, string, identificador ou expressão");
    return nullptr;
}
