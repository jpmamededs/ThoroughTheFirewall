@echo off
setlocal

:: CONFIGURAÇÕES
set IP_DESTINO=192.168.0.21
set ARQUIVO=senha.txt
set CAMINHO_ORIGEM=C:\caminho\para\%ARQUIVO%

:: VERIFICAÇÃO DE CONECTIVIDADE
ping -n 1 %IP_DESTINO% >nul
if %errorlevel% neq 0 (
    echo [ERRO] O computador de destino não está acessível.
    pause
    exit /b
)

:: OBTENDO O CAMINHO DA ÁREA DE TRABALHO DO DESTINO
echo [INFO] Obtendo o caminho da Área de Trabalho no destino...
for /f "tokens=3" %%d in ('wmic /node:%IP_DESTINO% process call create "cmd.exe /c echo %%USERPROFILE%%\Desktop" ^| findstr /i /c:"ReturnValue = 0"') do set DESKTOP_PATH=%%d

if "%DESKTOP_PATH%"=="" (
    echo [ERRO] Falha ao obter o caminho da Área de Trabalho.
    pause
    exit /b
)

echo [INFO] Caminho da Área de Trabalho no destino: %DESKTOP_PATH%

:: COPIANDO O ARQUIVO PARA A ÁREA DE TRABALHO DO DESTINO
echo [INFO] Enviando o arquivo para a Área de Trabalho do destino...
copy "%CAMINHO_ORIGEM%" "\\%IP_DESTINO%\%DESKTOP_PATH%" /Y
if %errorlevel% neq 0 (
    echo [ERRO] Falha ao copiar o arquivo. Verifique as permissões.
    pause
    exit /b
)

echo [SUCESSO] Arquivo enviado com sucesso para a Área de Trabalho do destino!
pause
endlocal
