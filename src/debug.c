#include "raylib.h"
#include "gemini.h"
#include "generalFunctions.h"
#include <string.h>
#include <stdio.h>
#include "playerStats.h"
#include "ranking.h"

static bool requisicaoIniciada = false;
static float tempoDecorrido = 0.0f;

void InitDebug(void) {
    requisicaoIniciada = false;
    tempoDecorrido = 0.0f;
}

void UpdateDebug(void) {

}

void DrawDebug(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    int   y          = 10;       /* coordenada Y inicial */
    const int lh     = 25;       /* altura de cada linha */
    char  info[256];             /* buffer p/ mensagens */

    /* ------------------------------------------------------------------ */
    /*  DADOS GERAIS                                                      */
    /* ------------------------------------------------------------------ */
    snprintf(info, sizeof(info), "Nome do Jogador: %s", playerStats.playerName);
    DrawText(info, 50, y, 20, GREEN);               y += lh;

    snprintf(info, sizeof(info), "Personagem: %s", playerStats.characterName);
    DrawText(info, 50, y, 20, GREEN);               y += lh;

    snprintf(info, sizeof(info), "Nota IA (0-80): %d", playerStats.aiOverallScore);
    DrawText(info, 50, y, 20, YELLOW);              y += lh;

    snprintf(info, sizeof(info), "Nota Geral: %.1f", playerStats.notalGeral);
    DrawText(info, 50, y, 20, ORANGE);              y += lh;

    DrawText(TextFormat("Status Seleção: %s",
             playerStats.isPassouSelecao ? "APROVADO" : "REPROVADO"),
             50, y, 20,
             playerStats.isPassouSelecao ? LIME : RED);
    y += lh * 2;   /* espaçamento extra antes dos desafios */

    /* ------------------------------------------------------------------ */
    /*  RESUMO DOS DESAFIOS                                               */
    /* ------------------------------------------------------------------ */
    DrawText("Resumo dos Desafios:", 50, y, 20, LIGHTGRAY);
    y += lh;

    snprintf(info, sizeof(info), "Desafio 01: %s | Duração: %d s",
             playerStats.isPassed_D01 ? "Passou" : "Falhou",
             playerStats.duration_D01);
    DrawText(info, 50, y, 20, WHITE);               y += lh;

    snprintf(info, sizeof(info), "Desafio 02: %s | Duração: %d s | Vidas: %d",
             playerStats.isPassed_D02 ? "Passou" : "Falhou",
             playerStats.duration_D02,
             playerStats.amountOfLives_D02);
    DrawText(info, 50, y, 20, WHITE);               y += lh;

    snprintf(info, sizeof(info), "Desafio 03: %s | Duração: %d s",
             playerStats.isPassed_D03 ? "Passou" : "Falhou",
             playerStats.duration_D03);
    DrawText(info, 50, y, 20, WHITE);               y += lh;

    snprintf(info, sizeof(info), "Desafio 04: %s | Ícones: %d",
             playerStats.isPassed_D04 ? "Passou" : "Falhou",
             playerStats.quantityOfIcons_D04);
    DrawText(info, 50, y, 20, WHITE);               y += lh * 2;

    /* ------------------------------------------------------------------ */
    /*  NOTAS POR PERGUNTA                                                */
    /* ------------------------------------------------------------------ */
    DrawText("Notas por Pergunta:", 50, y, 20, LIGHTGRAY);
    y += lh;

    for (int i = 0; i < MAX_PERGUNTAS; i++) {
        snprintf(info, sizeof(info), "Pergunta %d: %d", i + 1, notasIA[i]);
        DrawText(info, 50, y, 20, GREEN);
        y += lh;
    }

    y += lh;   /* espaço antes do relatório */

    /* ------------------------------------------------------------------ */
    /*  RELATÓRIO GERAL                                                   */
    /* ------------------------------------------------------------------ */
    DrawText("Relatório Geral:", 50, y, 20, LIGHTGRAY);
    y += lh;

    DrawText(playerStats.relatorioGeral, 50, y, 20, WHITE);

    EndDrawing();
}

void UnloadDebug(void)
{

}
