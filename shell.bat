@echo off
cd /d "%~dp0"
color 0A
title Verificação de Credenciais de CEO

:inicio
cls
echo    === VERIFICACAO DE CREDENCIAIS DE CEO ===
echo.
echo    Por favor, insira suas credenciais:
echo.

set /p "nome=Nome de Usuario: "
set /p "senha=Senha: "
set /p "credencial=Numero de Credencial (XXXX-X): "

if "%nome%"=="connorLewinsky@cybertech.cia" (
    if "%senha%"=="eo{96*GYC]k8.cia" (
        if "%credencial%"=="0000-1" (
            cls
            echo [INFO] Verificando credenciais...
            timeout /t 1 >nul
            
            echo [INFO] Autenticando... 30%%
            timeout /t 1 >nul
            
            echo [INFO] Verificando permissoes... 70%%
            timeout /t 1 >nul
            
            echo [INFO] Finalizando... 100%%
            timeout /t 1 >nul
            
            cls
            echo    === DADOS CONFIDENCIAIS ===
            echo.
            echo    * Projeto Blackbird: Status Ativo
            echo    * Codigo de Acesso Master: XK-7829
            echo    * Protocolo Phoenix: Iniciado em 15/03/2024
            echo    * Localizacoes das Bases: 
            echo      - 47.6062° N, 122.3321° W
            echo      - 34.0522° N, 118.2437° W
            echo      - 40.7128° N, 74.0060° W
            echo.
            echo    [ATENCAO] Estas informacoes sao classificadas
            echo.
            
            timeout /t 4 >nul
            echo ACESSO_CONCEDIDO > "%~dp0dadosShell.txt"
            exit
        )
    )
)

echo.
echo [ERRO] Credenciais invalidas. Tente novamente.
timeout /t 2 >nul
goto inicio