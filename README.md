# 🧠 Compilador/Interpretador Fortall

Este repositório contém o código-fonte de um **Compilador/Interpretador** da linguagem **Fortall**, desenvolvido em **C++**. O projeto abrange todas as fases clássicas da construção de um compilador: **análise léxica, sintática, semântica** e uma **fase de interpretação** para execução direta de código.

---

## 📁 Organização do Projeto

A estrutura de diretórios do projeto foi planejada para facilitar o desenvolvimento e a manutenção:

```
Compilador/
├── src/                # Código-fonte do compilador
│   ├── main.cpp
│   ├── lexer.cpp/.h
│   ├── parser.cpp/.h
│   ├── semantic.cpp/.h
│   ├── interpreter.cpp/.h
│   ├── symbol_table.cpp/.h
│   └── token.h
├── tests/              # Casos de teste em arquivos .fort
│   ├── test1.fort
│   ├── test2.fort
│   └── ...
├── bin/                # Local do executável gerado (fortall.exe)
│   └── fortall.exe
└── compile.bat         # Script de compilação e execução
```

---

## 📚 Gramática da Linguagem Fortall (EBNF)

A Fortall é inspirada em linguagens como C e Pascal, com foco em clareza e estrutura. Abaixo, a gramática principal:

<details>
<summary><strong>Clique para expandir a gramática</strong></summary>

```ebnf
---

## 📚 Gramática da Linguagem Fortall (EBNF)

A Fortall é inspirada em linguagens como C e Pascal, com foco em clareza e estrutura. Abaixo, a gramática principal, **ajustada para refletir a sintaxe implementada**:

<details>
<summary><strong>Clique para expandir a gramática</strong></summary>

```ebnf
<Programa>           ::= "programa" <Identificador> ";"
                       [ <SecaoVariaveis> ]
                       "inicio"
                       <ListaComandos>
                       "fim."

<SecaoVariaveis>     ::= "var" <ListaDeclaracoes>
<ListaDeclaracoes>   ::= <Declaracao> { <Declaracao> }
<Declaracao>         ::= <ListaIdentificadores> ":" <Tipo> ";"
<ListaIdentificadores>::= <Identificador> { "," <Identificador> }
<Tipo>               ::= "inteiro" | "logico"

<ListaComandos>      ::= { <Comando> }
<Comando>            ::= <Atribuicao> ";"
                       | <ChamadaEntrada> ";"
                       | <ChamadaSaida> ";"
                       | <Condicional>
                       | <LacoEnquanto>

<Atribuicao>         ::= <Identificador> ":=" <Expressao>
<ChamadaEntrada>     ::= "ler" "(" <ListaIdentificadores> ")"
<ChamadaSaida>       ::= "escrever" "(" <ListaExpressoes> ")"
<ListaExpressoes>    ::= <Expressao> { "," <Expressao> }

<Condicional>        ::= "se" "(" <Expressao> ")" "entao"
                       <ListaComandos>
                       [ "senao"
                       <ListaComandos> ]
                       "fim_se"

<LacoEnquanto>       ::= "enquanto" "(" <Expressao> ")" "faca"
                       <ListaComandos>
                       "fim_enquanto"

<Expressao>          ::= <ExpressaoRelacional>
<ExpressaoRelacional>::= <ExpressaoAritmetica> [ ( "==" | "!=" | "<" | ">" | "<=" | ">=" ) <ExpressaoAritmetica> ]
<ExpressaoAritmetica>::= <Termo> { ( "+" | "-" ) <Termo> }
<Termo>              ::= <Fator> { ( "*" | "/" ) <Fator> }
<Fator>              ::= <NumeroInteiro>
                       | <ValorBooleano>
                       | <Identificador>
                       | "(" <Expressao> ")"
                       | "-" <Fator>

<Identificador>      ::= <Letra> { <Letra> | <Digito> | "_" }
<NumeroInteiro>      ::= <Digito> { <Digito> }
<ValorBooleano>      ::= "verdadeiro" | "falso"

<Letra>              ::= 'a'..'z' | 'A'..'Z'
<Digito>             ::= '0'..'9'
```

> 💡 *Observações:*  
> - Todos os comandos (exceto blocos e estruturas de controle) terminam com `;`  
> - Tipos básicos suportados: `inteiro`, `logico` 
> - Suporte a operadores aritméticos, relacionais e o operador unário de negação (-) para inteiros.
> - Condicionais e laços exigem expressões lógicas entre parênteses.

</details>

---

## 🛠️ Compilação e Execução

### ⚙️ Pré-requisitos
- **G++** instalado (ex: [MinGW-w64](http://mingw-w64.org/))
- **Windows** com acesso ao terminal (`cmd.exe` ou `PowerShell`)

Verifique o G++:
```bash
g++ --version
```

### 🚀 Etapas para Compilar

No terminal:

```bash
cd caminho/para/Compilador
.\compile.bat
```

⚠️ Se usar PowerShell, execute via `cmd`:

```bash
cmd /c .\compile.bat
```

---

## 📋 Menu de Execução

O `compile.bat` oferece um menu interativo após a compilação:

| Opção | Ação                                                                 |
|-------|----------------------------------------------------------------------|
| 1     | Executa **todos** os arquivos `.fort` dentro de `tests/`            |
| 2     | Executa um **teste específico** (ex: `test1.fort`)                  |
| 3     | Executa um **arquivo `.fort` personalizado**                        |
| 4     | Limpa (`del bin\fortall.exe`) para recompilar do zero              |
| 0     | Encerra o script                                                    |

---

## 🧩 Detalhes da Implementação

### 🔹 Análise Léxica (Lexer)
- Implementado em `lexer.cpp/.h`
- Baseado em **AFD manual**
- Produz uma lista de tokens
- Valida caracteres e reporta erros léxicos

### 🔹 Análise Sintática (Parser)
- Implementado em `parser.cpp/.h`
- Parsing **recursivo descendente**
- Gera a **AST (Abstract Syntax Tree)**
- Reporta e tenta recuperar de erros sintáticos

### 🔹 Análise Semântica (Semantic Analyzer)
- Implementado em `semantic.cpp/.h`
- Usa **tabela de símbolos** (`symbol_table.cpp/.h`)
- Verifica declarações, tipos e escopo
- Realiza a verificação de tipos para atribuições, expressões e condições de controle de fluxo (se, enquanto).
- Garante que operações relacionais (==, !=, >, <, >=, <=) resultem em valores lógicos (LOGICO) e que operações aritméticas resultem em inteiros.
- Detecta o uso de variáveis não declaradas.
- A verificação de inicialização de variáveis antes de seu uso em expressões é delegada à fase de Interpretação, para maior flexibilidade e precisão em tempo de execução, especialmente para variáveis lidas via ler.
- Popula informações das variáveis e gera erros semânticos

### 🔹 Interpretação (Interpreter)
- Implementado em `interpreter.cpp/.h`
- Executa a **AST validada**
- Suporta expressões, comandos, controle de fluxo
- Simula memória com a tabela de símbolos
- É responsável por inicializar variáveis quando um valor é atribuído ou lido (:=, ler), e por reportar erros de uso de variáveis não inicializadas durante a execução.
- Reporta erros em tempo de execução (ex: divisão por zero)

---

## ✅ Exemplo de Execução

```bash
> Escolha uma opcao para executar seu compilador:
> 1. Executar TODOS os testes (.fort)
> 2. Executar um TESTE ESPECIFICO (.fort)
> 3. Compilar e executar um ARQUIVO .fort
> 4. Limpar arquivos de compilacao (executavel)
> 0. Sair
```

---
