@echo off
setlocal enabledelayedexpansion

:: ==============================================
:: === COMPILADOR FORTALL - SCRIPT DE BUILD ===
:: ==============================================

:: --- 1. Verificacao do Compilador G++ (REMOVIDA PARA EVITAR ERROS) ---
:: Esta seção foi removida/comentada para garantir a execução sem erros.
:: O script assume que o G++ está instalado e configurado no PATH.

:: --- 2. Compila o projeto Fortall ---
echo.
echo Compilando o Compilador Fortall...
echo.

:: Define os arquivos fonte (na pasta src/)
set "SOURCES=src/main.cpp src/lexer.cpp src/parser.cpp src/semantic.cpp src/interpreter.cpp src/symbol_table.cpp"
:: Define o caminho para o executavel de saida (na pasta bin/)
set "OUTPUT_EXE=bin\fortall.exe"
:: Define as flags do compilador
set "COMPILER_FLAGS=-std=c++17 -O2 -Wall -Wextra"

:: Cria a pasta bin/ se nao existir
if not exist bin\ (
    mkdir bin
    echo Criando diretorio bin/
)

:: Executa a compilacao
g++ %SOURCES% %COMPILER_FLAGS% -o "%OUTPUT_EXE%"

if %errorlevel% neq 0 (
    echo.
    echo ERRO: Falha na compilacao do Fortall!
    echo Verifique os erros acima.
    echo.
    pause
    goto :eof
)

echo.
echo === COMPILACAO BEM-SUCEDIDA! ===
echo Executavel gerado: %OUTPUT_EXE%
echo.

:: =======================================
:: === OPCOES DE EXECUCAO DO COMPILADOR ===
:: =======================================

:MENU
echo.
echo Escolha uma opcao para executar seu compilador:
echo   1. Executar TODOS os testes (.fort)
echo   2. Executar um TESTE ESPECIFICO (.fort)
echo   3. Compilar e executar um ARQUIVO .fort (informar o caminho)
echo   4. Limpar arquivos de compilacao (executavel)
echo   0. Sair
echo.

set /p CHOICE="Digite sua escolha (0-4): "

if "%CHOICE%"=="1" goto RUN_ALL_TESTS
if "%CHOICE%"=="2" goto RUN_SPECIFIC_TEST
if "%CHOCHOICE%"=="3" goto COMPILE_AND_RUN_FILE
if "%CHOICE%"=="4" goto CLEAN_BUILD
if "%CHOICE%"=="0" goto :eof

echo.
echo Opcao invalida. Por favor, digite 0, 1, 2, 3 ou 4.
goto MENU

:: --- Opcao 1: Executar todos os testes ---
:RUN_ALL_TESTS
echo.
echo === EXECUTANDO TODOS OS TESTES ===
echo.
"%OUTPUT_EXE%" test
goto :END_EXECUTION

:: --- Opcao 2: Executar um teste especifico ---
:RUN_SPECIFIC_TEST
echo.
echo === EXECUTANDO TESTE ESPECIFICO ===
echo.
set /p "TEST_NUM=Qual numero do teste deseja executar (ex: 1 para test1.fort): "

:: Verificamos se a entrada não está vazia.
:: Usamos %TEST_NUM% aqui pois está fora de um bloco complexo, ou seja,
:: a variável já deveria ter sido expandida.
if "%TEST_NUM%"=="" (
    echo Nenhum numero de teste informado.
    goto :END_EXECUTION
)

:: Se o número foi informado, construímos o caminho e executamos.
:: Agora, como estamos em uma nova "linha de execução" lógica,
:: !TEST_NUM! deve expandir corretamente.
set "TEST_FILE_PATH=tests\test%TEST_NUM%.fort"
echo Verificando: "%TEST_FILE_PATH%"

if exist "%TEST_FILE_PATH%" (
    echo Executando "%TEST_FILE_PATH%"...
    "%OUTPUT_EXE%" "%TEST_FILE_PATH%"
) else (
    echo ERRO: Arquivo "%TEST_FILE_PATH%" nao encontrado na pasta tests/.
    echo O sistema nao pode encontrar o arquivo especificado. (Esta mensagem nao eh do sistema)
)
goto :END_EXECUTION

:: --- Opcao 3: Compilar e executar um arquivo .fort ---
:COMPILE_AND_RUN_FILE
echo.
echo === EXECUTANDO ARQUIVO .FORT ESPECIFICO ===
echo.
set /p "FORT_FILE_INPUT=Digite o caminho para o arquivo .fort (ex: tests/meu_programa.fort ou programa.fort): "

if "%FORT_FILE_INPUT%"=="" (
    echo Nenhum arquivo informado.
    goto :END_EXECUTION
)

if exist "%FORT_FILE_INPUT%" (
    echo Executando "%FORT_FILE_INPUT%"...
    "%OUTPUT_EXE%" "%FORT_FILE_INPUT%"
) else (
    echo ERRO: Arquivo '%FORT_FILE_INPUT%' nao encontrado.
    echo Certifique-se de que o caminho esta correto (ex: 'tests/meu_programa.fort').
    echo O sistema nao pode encontrar o arquivo especificado. (Esta mensagem nao eh do sistema)
)
goto :END_EXECUTION

:: --- Opcao 4: Limpar arquivos de compilacao ---
:CLEAN_BUILD
echo.
echo Limpando arquivos de compilacao...
if exist "%OUTPUT_EXE%" (
    del "%OUTPUT_EXE%"
    echo Arquivo "%OUTPUT_EXE%" removido.
) else (
    echo Nenhum executavel para remover na pasta bin/.
)
echo.
goto :END_EXECUTION

:END_EXECUTION
echo.
pause
endlocal