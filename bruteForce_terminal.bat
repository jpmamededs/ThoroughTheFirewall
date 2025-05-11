@echo off
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
echo.
echo Iniciando...
timeout /t 2 >nul

:inicio
set /p comando=Insira os usernames da wordlist: 

:: Lista de usernames válidos
set usernames=admin user123 hankmicucci usertest superuser hank_123 usradm cybertechinc

:: Verificar se o username está na lista
for %%u in (%usernames%) do if /i "%%u"=="%comando%" goto senha

echo [ERRO] Username nao reconhecido. Tente novamente.
goto inicio

:senha
set /p senha=Insira as senhas da wordlist: 

:: Lista de senhas válidas
set senhas=@Password passkey 19021986.Hank hank123 cybertech@1 admin passwd#Su123 passwd

:: Simulação de tentativa
set max=64
set tentativa=0
cls
echo [INFO] Iniciando brute-force...
for %%a in (%usernames%) do for %%b in (%senhas%) do (
    set /a tentativa+=1
    set /a progress=(tentativa*100)/max
    cls
    echo [INFO] Tentativa %%a / %%b
    echo [INFO] Progresso: !progress!%%
    if "%%a"=="superuser" if "%%b"=="passwd#Su123" (
        echo.
        echo [SUCESSO] Acesso concedido com username: superuser e senha: passwd#Su123
        timeout /t 3 >nul
        exit
    )
)

echo [FALHA] Nenhuma combinacao encontrada.
timeout /t 2 >nul
exit
