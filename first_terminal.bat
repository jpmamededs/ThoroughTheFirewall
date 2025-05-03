@echo off
cd /d "%~dp0"
color 0a
title Terminal Hacker
cls

echo Lembrete: Quando for finalizar aquela sua tarefa, digite run-virus
echo.

:inicio
set /p comando=Digite o comando: 

if /i "%comando%"=="run-virus" (
    cls
    echo Iniciando protocolo...
    timeout /t 1 >nul

    echo Carregando... 10%%
    timeout /t 1 >nul

    echo Carregando... 40%%
    timeout /t 1 >nul

    echo Carregando... 80%%
    timeout /t 1 >nul

    echo Carregando... 100%%
    timeout /t 1 >nul

    echo.
    echo Processo finalizado com sucesso.

    :: Cria ou sobrescreve dados.txt na pasta atual
    echo OK > "%~dp0dados.txt"

    timeout /t 3 >nul
    exit
) else (
    echo Comando incorreto. Tente novamente.
    timeout /t 1 >nul
    goto inicio
)
