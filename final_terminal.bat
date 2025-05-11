@echo off
cd /d "%~dp0"
color 0a
title Terminal de Teste - FBI
cls

echo Terminal de Teste - FBI.
echo.
echo ██████  ███████  ██████  ██████   ██████ ███████ ██████ .
echo ██   ██ ██      ██    ██ ██   ██ ██      ██      ██   ██ .
echo ██████  █████   ██    ██ ██████  ██      █████   ██████  .
echo ██   ██ ██      ██    ██ ██   ██ ██      ██      ██   ██.
echo ██████  ██       ██████  ██   ██  ██████ ███████ ██   ██ .
echo.

:inicio
set /p comando=Insira os usernames da wordlist: 

if /i "%comando%"=="passar firewall" (
    cls
    echo [INFO] Verificando habilidades...
    timeout /t 1 >nul

    echo [INFO] Protocolo iniciado... 10%%
    timeout /t 1 >nul

    echo [INFO] Protocolo em progresso... 40%%
    timeout /t 1 >nul

    echo [INFO] Protocolo quase completo... 80%%
    timeout /t 1 >nul

    echo [INFO] Finalizando... 100%%
    timeout /t 1 >nul

    echo.
    echo [SUCESSO] Firewall ultrapassado. Teste inicial concluído.
    echo.

    :: Cria ou sobrescreve dados.txt na pasta atual
    echo OK > "%~dp0bruteforcer.txt"

    timeout /t 3 >nul
    exit
) else (
    color 0c
    echo [ERRO] Comando nao reconhecido. Tente novamente.
    color 0a
    goto inicio
)
