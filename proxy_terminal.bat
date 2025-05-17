@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
color 0a
title Terminal de Teste - FBI
cls

:: ASCII Art Inicial
echo Terminal de Teste - FBI.
echo.
echo ██████  ███████  ██████  ██████   ██████ ███████ ██████ .
echo ██   ██ ██      ██    ██ ██   ██ ██      ██      ██   ██ .
echo ██████  █████   ██    ██ ██████  ██      █████   ██████  .
echo ██   ██ ██      ██    ██ ██   ██ ██      ██      ██   ██.
echo ██████  ██       ██████  ██   ██  ██████ ███████ ██   ██ .
echo.

:: Solicita e valida URL do site antes de prosseguir
:AskURL
set /p siteURL=Insira a Porta HTTP: 
if /i not "!siteURL!"=="3128" (
    echo [ERRO] Porta invalida. Tente novamente.
    goto AskURL
)

:: Solicita path de users.txt até ser válido
:AskUsers
set /p usersFile=Insira a acl localnet src: 
if /i not "!usersFile!"=="192.168.1.0/24" (
    echo [ERRO] 'acl localnet src' invalida. Tente novamente.
    goto AskUsers
)

:: Solicita path de passwords.txt até ser válido
:AskPasswords
set /p passFile=Insira a configuracao de acesso HTTP: 
if /i not "!passFile!"=="allow localnet" (
    echo [ERRO] Configuracao de acesso HTTP invalida. Tente novamente.
    goto AskPasswords
)

:: Mensagem de sucesso
echo.
echo Servidor proxy configurado com sucesso! Fechando processo...
echo OK > "%~dp0dadosProxy.txt"
ping -n 4 127.0.0.1 > nul  :: Pausa por 3 segundos

:: Fecha o terminal
exit