#include "raylib.h"
#include "gemini.h"
#include "generalFunctions.h"
#include <string.h>
#include <stdio.h>

static char resposta[1024] = {0};
static bool requisicaoIniciada = false;
//static float tempoParaIniciar = 0.5f; // atraso de meio segundo
static float tempoDecorrido = 0.0f;

void InitDebug(void) {
    requisicaoIniciada = false;
    tempoDecorrido = 0.0f;
    memset(resposta, 0, sizeof(resposta));
}

void UpdateDebug(void) {
    tempoDecorrido += GetFrameTime();

    int yOffset = 120;
    for (int i = 0; i < MAX_PERGUNTAS; i++) {
        if (notasIA[i] >= 0) {
            char buf[1024];
            snprintf(buf, sizeof(buf), "[%d] NOTA: %d", i, notasIA[i]);
            DrawText(buf, 20, yOffset, 20, GREEN);
            yOffset += 24;

            snprintf(buf, sizeof(buf), "RELATORIO: %s", relatoriosIA[i]);
            DrawText(buf, 40, yOffset, 20, GRAY);
            yOffset += 40;
        }
    }
}

void DrawDebug(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Desenha o nome do jogador no topo da tela
    DrawText("Nome do Jogador:", 50, 10, 20, LIGHTGRAY);
    DrawText(gPlayerName, 250, 10, 20, GREEN);

    int y = 80;
    const int lineHeight = 25;

    char respostaTemp[1024];
    strncpy(respostaTemp, resposta, sizeof(respostaTemp));

    char *linha1 = strtok(respostaTemp, "\n");
    while (linha1 != NULL) {
        DrawText(linha1, 50, y, 20, WHITE);
        y += lineHeight;
        linha1 = strtok(NULL, "\n");
    }

    // Mostrar a nota total calculada
    int notaTotal = SomarNotasIA();
    char notaStr[64];
    snprintf(notaStr, sizeof(notaStr), "Nota Total da IA: %d", notaTotal);
    DrawText(notaStr, 50, y, 20, YELLOW);
    y += lineHeight;

    // Gerar o relatório geral e exibir
    char relatorioFinal[1024];
    GerarRelatorioGeralIA(relatorioFinal, sizeof(relatorioFinal));
    DrawText("Relatório Geral:", 50, y, 20, LIGHTGRAY);
    y += lineHeight;

    // Quebra o relatório final em várias linhas para exibição
    char relatorioTemp[1024];
    strncpy(relatorioTemp, relatorioFinal, sizeof(relatorioTemp));
    char *linha = strtok(relatorioTemp, "\n");
    while (linha != NULL) {
        DrawText(linha, 50, y, 20, WHITE);
        y += lineHeight;
        linha = strtok(NULL, "\n");
    }

    EndDrawing();
}

void UnloadDebug(void) {
    // Nada a descarregar no momento
}
