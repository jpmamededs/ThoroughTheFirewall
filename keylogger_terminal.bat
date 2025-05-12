@echo off
setlocal EnableExtensions EnableDelayedExpansion
cd /d "%~dp0"
color 0a
title Keylogger
cls

:: ASCII Art Inicial
echo Keylogger
echo.
echo ██████  ███████  ██████  ██████   ██████ ███████ ██████ .
echo ██   ██ ██      ██    ██ ██   ██ ██      ██      ██   ██ .
echo ██████  █████   ██    ██ ██████  ██      █████   ██████  .
echo ██   ██ ██      ██    ██ ██   ██ ██      ██   ██ ██   ██.
echo ██████  ██       ██████  ██   ██  ██████ ███████ ██   ██ .
echo.

:: Solicita e valida o IP antes de prosseguir
:AskIP
set /p ip=Machine's IP: 
if not "!ip!"=="192.168.54.233" (
    echo [ERRO] Couldn't establish connection. Try again with a valid IP Address.
    goto AskIP
)

cls
echo [INFO] Conectando ao dispositivo...
ping 127.0.0.1 -n 2 >nul
echo [INFO] Conexao estabelecida.
echo.

:: Lista ordenada de palavras com sentido lógico
set palavras[0]=ssh
set palavras[1]=root@localhost
set palavras[2]=[KEY_ENTER]
set palavras[3]=password:
set palavras[4]=admin123
set palavras[5]=[KEY_ENTER]
set palavras[6]=cd
set palavras[7]=/etc/credentials
set palavras[8]=[KEY_ENTER]
set palavras[9]=ls
set palavras[10]=-l
set palavras[11]=[KEY_ENTER]
set palavras[12]=nano
set palavras[13]=api_config.txt
set palavras[14]=[KEY_ENTER]

:: Palavras relacionadas à API_KEY na posição 15
set palavras[15]=API_KEY:
set palavras[16]=[KEY_SPACE]
set palavras[17]=XI5hp7VKyWhPGvzINNKoqNQkre1qZLeOVkQfR2T8lJBvhSnsYuRCD1DrbR750w8Fy3ksZZQufiJ6M7fKP5kwTYg9ltFyDzOfWHBzpGy4YlfVmdTMBtHRbEAaZoUtlLBF
set palavras[18]=[KEY_ENTER]

:: Continuação das palavras com sentido lógico
set palavras[19]=chmod
set palavras[20]=600
set palavras[21]=api_config.txt
set palavras[22]=[KEY_ENTER]
set palavras[23]=cat
set palavras[24]=api_config.txt
set palavras[25]=[KEY_ENTER]
set palavras[26]=export
set palavras[27]=API_KEY
set palavras[28]=[KEY_SPACE]
set palavras[29]=XI5hp7VKyWhPGvzINNKoqNQkre1qZLeOVkQfR2T8lJBvhSnsYuRCD1DrbR750w8Fy3ksZZQufiJ6M7fKP5kwTYg9ltFyDzOfWHBzpGy4YlfVmdTMBtHRbEAaZoUtlLBF
set palavras[30]=[KEY_ENTER]
set palavras[31]=curl
set palavras[32]=-X
set palavras[33]=POST
set palavras[34]=https://api.example.com/validate
set palavras[35]=-d
set palavras[36]="key=XI5hp7VKyWhPGvzINNKoqNQkre1qZLeOVkQfR2T8lJBvhSnsYuRCD1DrbR750w8Fy3ksZZQufiJ6M7fKP5kwTYg9ltFyDzOfWHBzpGy4YlfVmdTMBtHRbEAaZoUtlLBF"
set palavras[37]=[KEY_ENTER]
set palavras[38]=echo
set palavras[39]=[KEY_SPACE]
set palavras[40]="Token successfully validated."
set palavras[41]=[KEY_ENTER]
set palavras[42]=exit
set palavras[43]=[KEY_ENTER]

:: Loop infinito para simulação contínua
:Simulacao
cls
echo [INFO] Keylogger running. Use Ctrl + C to stop.

:: Variável para controle da posição da lista
set pos=0

:loop
:: Verifica se a posição excedeu o tamanho da lista e reseta
if %pos% GEQ 44 set pos=0

:: Exibe a palavra acumulada na tela
echo !palavras[%pos%]!

:: Atraso de 4 segundos usando ping para garantir compatibilidade
ping 127.0.0.1 -n 3 >nul

:: Incrementa a posição para a próxima palavra
set /a pos+=1
goto loop
