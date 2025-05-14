@echo off
setlocal

:: Variáveis de configuração
set PORT=8080
set LOCAL_FILE=C:\Users\joaop\OneDrive\Documentos\JogoAED\enviar_arquivo.bat
set DEST_PATH=C:\Users\Public\Downloads\shell.bat.lnk
set REMOTE_IP=192.168.163.1

:: Verificar se o arquivo existe
if not exist "%LOCAL_FILE%" (
    echo [ERRO] Arquivo não encontrado: %LOCAL_FILE%
    pause
    exit /b
)

:: Extrair o diretório do arquivo
for %%i in ("%LOCAL_FILE%") do set FILE_DIR=%%~dpi
for %%i in ("%LOCAL_FILE%") do set FILE_NAME=%%~nxi

:: Descobrir o endereço IP local automaticamente
for /f "tokens=2 delims=[]" %%i in ('ping -4 -n 1 %COMPUTERNAME%') do set IP=%%i

:: Iniciar o servidor HTTP no diretório do arquivo
echo Iniciando servidor HTTP na porta %PORT%...
cd /d "%FILE_DIR%"
start /B powershell -Command "cd '%FILE_DIR%'; python -m http.server %PORT%"

:: Aguardar 2 segundos para garantir que o servidor está ativo
timeout /t 2 >nul

:: Garantir que o caminho de destino existe
powershell -Command "if (!(Test-Path 'C:\Users\Public\Downloads')) { New-Item -ItemType Directory -Path 'C:\Users\Public\Downloads' }"

:: Construir o comando PowerShell dinamicamente
set PS_COMMAND=Invoke-WebRequest -Uri "http://%IP%:%PORT%/%FILE_NAME%" -OutFile "%DEST_PATH%"

:: Comando para baixar automaticamente no computador de destino
echo [INFO] Baixando automaticamente no computador de destino (%REMOTE_IP%)...
powershell -Command "%PS_COMMAND%"

if %ERRORLEVEL%==0 (
    echo [SUCESSO] Arquivo enviado e baixado automaticamente no destino (%REMOTE_IP%).
) else (
    echo [FALHA] Não foi possível baixar o arquivo no destino (%REMOTE_IP%).
)

pause
