#include "raylib.h"
#include "gemini.h"
#include "generalFunctions.h"
#include <string.h>
#include <stdio.h>

static char resposta[1024] = {0};
static bool respostaPronta = false;
static bool requisicaoIniciada = false;
//static float tempoParaIniciar = 0.5f; // atraso de meio segundo
static float tempoDecorrido = 0.0f;

void InitDebug(void) {
    respostaPronta = false;
    requisicaoIniciada = false;
    tempoDecorrido = 0.0f;
    memset(resposta, 0, sizeof(resposta));
}

void UpdateDebug(void) {
    tempoDecorrido += GetFrameTime();

    //if (!requisicaoIniciada && tempoDecorrido >= tempoParaIniciar) {
    //    ObterRespostaGemini(
    //        "Você é um detetive. O que deve ser feito agora?",
    //        resposta
    //    );
    //    respostaPronta = true;
    //    requisicaoIniciada = true;
    //}

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

    if (!respostaPronta) {
        DrawText("Carregando resposta da IA...", 50, 50, 20, GRAY);
    } else {
        int y = 50;
        const int lineHeight = 25;

        char respostaTemp[1024];
        strncpy(respostaTemp, resposta, sizeof(respostaTemp));

        char *linha = strtok(respostaTemp, "\n");
        while (linha != NULL) {
            DrawText(linha, 50, y, 20, WHITE);
            y += lineHeight;
            linha = strtok(NULL, "\n");
        }
    }

    EndDrawing();
}

void UnloadDebug(void) {
    // Nada a descarregar no momento
}
