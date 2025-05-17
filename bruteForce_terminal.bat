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
set /p siteURL=Insira a URL do site: 
if /i not "!siteURL!"=="https://cybertechinc.vercel.app/" (
    echo [ERRO] URL invalida. Tente novamente.
    goto AskURL
)

:: Solicita path de users.txt até ser válido
:AskUsers
set /p usersFile=Insira o caminho para users.txt: 
set "usersFile=!usersFile:"=! "
if not exist "!usersFile!" (
    echo [ERRO] Arquivo "!usersFile!" nao encontrado.
    goto AskUsers
)

:: Solicita path de passwords.txt até ser válido
:AskPasswords
set /p passFile=Insira o caminho para passwords.txt: 
set "passFile=!passFile:"=! "
if not exist "!passFile!" (
    echo [ERRO] Arquivo "!passFile!" nao encontrado.
    goto AskPasswords
)

:: Inicializa contadores e barra
set tentativa=0
set "bar="

cls
echo [INFO] Iniciando brute-force...
echo.

:: Loop de brute-force com contagem, barra visual e delay
for /f "usebackq delims=" %%u in ("!usersFile!") do (
    for /f "usebackq delims=" %%p in ("!passFile!") do (
        set /a tentativa+=1
        set "bar=!bar!#"
        cls
        echo [INFO] [!bar!]
        echo [INFO] Tentativa !tentativa!: %%u / %%p

        :: Pequeno delay para visualizar cada "frame"
        ping 127.0.0.1 -n 1 >nul

        :: Verifica credenciais corretas
        if /i "%%u"=="superuser" if "%%p"=="passwd#Su123" (
            echo.
            echo credentials found: superuser ^& passwd#Su123
            echo [INFO] Atualizando...
            ping 127.0.0.1 -n 2 >nul
            goto AskSecretKey
        )
    )
)

:AskSecretKey
cls
echo [INFO] Secret Base64 Encrypted ID Key:
set /p secretKey=Digite a chave secreta: 
if /i "!secretKey!"=="dGVzdCBwYXNzZWQ=" (
    echo [INFO] Decrypted key: 'test passed'
    echo test passed > dadosBruteForce.txt
    ping 127.0.0.1 -n 3 >nul
    exit /b
) else (
    echo [ERRO] Chave incorreta. Tente novamente.
    goto AskSecretKey
)

:End
endlocal
exit /b
