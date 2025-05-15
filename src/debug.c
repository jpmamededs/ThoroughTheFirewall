#include "raylib.h"
#include "gemini.h"
#include "generalFunctions.h"
#include <string.h>
#include <stdio.h>
#include "playerStats.h"

static char resposta[1024] = {0};
static bool requisicaoIniciada = false;
static float tempoDecorrido = 0.0f;

void InitDebug(void) {
    requisicaoIniciada = false;
    tempoDecorrido = 0.0f;
    memset(resposta, 0, sizeof(resposta));
}

void UpdateDebug(void) {

}


void DrawDebug(void) 
{
    BeginDrawing();
    ClearBackground(BLACK);

    int y = 10;
    const int lineHeight = 25;

    // Dados gerais do jogador
    char info[256];

    snprintf(info, sizeof(info), "Nome do Jogador: %s", playerStats.playerName);
    DrawText(info, 50, y, 20, GREEN);
    y += lineHeight;

    snprintf(info, sizeof(info), "Personagem: %s", playerStats.characterName);
    DrawText(info, 50, y, 20, GREEN);
    y += lineHeight;

    snprintf(info, sizeof(info), "Nota Final da IA: %d", playerStats.aiOverallScore);
    DrawText(info, 50, y, 20, YELLOW);
    y += lineHeight;

    DrawText("Resumo dos Desafios:", 50, y, 20, LIGHTGRAY);
    y += lineHeight;

    // Dados do Desafio 01
    snprintf(info, sizeof(info), "Desafio 01: %s | Duração: %d segundos", 
             playerStats.isPassed_D01 ? "Passou" : "Falhou", playerStats.duration_D01);
    DrawText(info, 50, y, 20, WHITE);
    y += lineHeight;

    // Dados do Desafio 02
    snprintf(info, sizeof(info), "Desafio 02: %s | Duração: %d segundos | Vidas Restantes: %d",
             playerStats.isPassed_D02 ? "Passou" : "Falhou", 
             playerStats.duration_D02, playerStats.amountOfLives_D02);
    DrawText(info, 50, y, 20, WHITE);
    y += lineHeight;

    // Dados do Desafio 03
    snprintf(info, sizeof(info), "Desafio 03: %s | Duração: %d segundos", 
             playerStats.isPassed_D03 ? "Passou" : "Falhou", playerStats.duration_D03);
    DrawText(info, 50, y, 20, WHITE);
    y += lineHeight;

    // Dados do Desafio 04
    snprintf(info, sizeof(info), "Desafio 04: %s | Ícones Coletados: %d", 
             playerStats.isPassed_D04 ? "Passou" : "Falhou", playerStats.quantityOfIcons_D04);
    DrawText(info, 50, y, 20, WHITE);
    y += lineHeight;

    // Mostrar a nota total calculada
    int notaTotal = SomarNotasIA();
    snprintf(info, sizeof(info), "Nota Total da IA: %d", notaTotal);
    DrawText(info, 50, y, 20, YELLOW);
    y += lineHeight * 2;  // Espaçamento adicional entre seções

    // Exibir notas detalhadas de cada pergunta
    DrawText("Notas por Pergunta:", 50, y, 20, LIGHTGRAY);
    y += lineHeight;

    for (int i = 0; i < MAX_PERGUNTAS; i++) {
        if (notasIA[i] >= 0) {
            snprintf(info, sizeof(info), "Pergunta %d: Nota %d", i + 1, notasIA[i]);
            DrawText(info, 50, y, 20, GREEN);
            y += lineHeight;
        }
    }

    y += lineHeight * 2;  // Espaçamento para o relatório

    // Exibir o relatório geral
    char relatorioFinal[1024];
    GerarRelatorioGeralIA(relatorioFinal, sizeof(relatorioFinal));
    DrawText("Relatório Geral:", 50, y, 20, LIGHTGRAY);
    y += lineHeight;

    // Quebrar relatório em várias linhas
    char relatorioTemp[1024];
    strncpy(relatorioTemp, relatorioFinal, sizeof(relatorioTemp));
    char *linha = strtok(relatorioTemp, "\n");
    while (linha != NULL) 
    {
        DrawText(linha, 50, y, 20, WHITE);
        y += lineHeight;
        linha = strtok(NULL, "\n");
    }

    EndDrawing();
}

void UnloadDebug(void) {
    // Nada a descarregar no momento
}
