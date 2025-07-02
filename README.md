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
<Programa>         ::= <Bloco>
<Bloco>            ::= "{" { <Declaracao> | <Comando> } "}"
<Declaracao>       ::= "var" <Identificador> ":" <Tipo> ";"
<Tipo>             ::= "int" | "float" | "bool"
<Comando>          ::= <Atribuicao> ";"
                   |   <Entrada> ";"
                   |   <Saida> ";"
                   |   <Condicional>
                   |   <LacoEnquanto>
                   |   <Bloco>
<Atribuicao>       ::= <Identificador> "=" <Expressao>
<Entrada>          ::= "leia" <Identificador>
<Saida>            ::= "escreva" <Expressao>
<Condicional>      ::= "se" "(" <Expressao> ")" <Comando> [ "senao" <Comando> ]
<LacoEnquanto>     ::= "enquanto" "(" <Expressao> ")" <Comando>

<Expressao>        ::= <ExpressaoBooleana>
<ExpressaoBooleana>::= <ExpressaoAritmetica> [ ( "==" | "!=" | "<" | ">" | "<=" | ">=" ) <ExpressaoAritmetica> ]
<ExpressaoAritmetica>::= <Termo> { ( "+" | "-" ) <Termo> }
<Termo>            ::= <Fator> { ( "*" | "/" ) <Fator> }
<Fator>            ::= <NumeroInteiro>
                   |   <NumeroFloat>
                   |   <ValorBooleano>
                   |   <Identificador>
                   |   "(" <Expressao> ")"
                   |   ( "-" | "!" ) <Fator>

<Identificador>    ::= <Letra> { <Letra> | <Digito> | "_" }
<NumeroInteiro>    ::= <Digito> { <Digito> }
<NumeroFloat>      ::= <NumeroInteiro> "." <NumeroInteiro>
<ValorBooleano>    ::= "verdadeiro" | "falso"

<Letra>            ::= 'a'..'z' | 'A'..'Z'
<Digito>           ::= '0'..'9'
```

> 💡 *Observações:*  
> - Todos os comandos (exceto blocos e estruturas de controle) terminam com `;`  
> - Tipos básicos suportados: `int`, `float`, `bool`  
> - Suporte a operadores aritméticos, relacionais e unários

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
- Popula informações das variáveis e gera erros semânticos

### 🔹 Interpretação (Interpreter)
- Implementado em `interpreter.cpp/.h`
- Executa a **AST validada**
- Suporta expressões, comandos, controle de fluxo
- Simula memória com a tabela de símbolos
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