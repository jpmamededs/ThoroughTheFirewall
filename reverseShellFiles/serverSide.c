#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32")

#define PORT 8081
#define BUFFER_SIZE 4096

int main() {
    WSADATA wsa;
    SOCKET listener, client;
    struct sockaddr_in server, client_addr;
    int client_len = sizeof(client_addr);
    char command[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int bytes_received;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        printf("[!] WSAStartup failed\n");
        return 1;
    }

    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("[!] Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(listener, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        printf("[!] Bind failed\n");
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    listen(listener, 1);
    printf("[*] Esperando conexões na porta %d...\n", PORT);

    client = accept(listener, (struct sockaddr *)&client_addr, &client_len);
    if (client == INVALID_SOCKET) {
        printf("[!] Accept failed\n");
        closesocket(listener);
        WSACleanup();
        return 1;
    }

    printf("[+] Conexão estabelecida de %s\n", inet_ntoa(client_addr.sin_addr));
    printf("[*] Digite 'help' para ver os comandos\n\n");

    while (1) {
        printf("Shell> ");
        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "exit") == 0) break;
        
        if (strcmp(command, "help") == 0) {
            printf("\nComandos:\n");
            printf("cd [dir]    - Mudar o diretório e ver o novo caminho\n");
            printf("cd..        - Voltar um diretório e ver o novo caminho\n");
            printf("dir         - Listar os arquivos dentro de um diretório\n");
            printf("pwd         - Mostrar o diretório atual\n");
            printf("exit        - Sair\n\n");
            continue;
        }

        if (send(client, command, strlen(command), 0) <= 0) {
            printf("[!] Conexão perdida\n");
            break;
        }

        while ((bytes_received = recv(client, response, BUFFER_SIZE-1, 0)) > 0) {
            response[bytes_received] = '\0';
            printf("%s", response);
            if (strstr(response, "[CMD_END]")) break;
        }

        if (bytes_received <= 0) {
            printf("[!] Erro de conexão\n");
            break;
        }
    }

    closesocket(client);
    closesocket(listener);
    WSACleanup();
    return 0;
}