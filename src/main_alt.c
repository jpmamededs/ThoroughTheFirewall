/*  main_alt.c  –– Versão enxuta:
    • Executa apenas 4 rodadas de INTERROGATORIO
    • Calcula o resultado final do jogador
    • Entra em APP_DEBUG para mostrar o relatório          

#include "interrogatorio.h"
#include "debug.h"
#include "playerStats.h"
#include "generalFunctions.h"
#include "raylib.h"
#include "ranking.h"

#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

static AppState state          = INTERROGATORIO;
PlayerStats playerStats = {0};

typedef struct { const char *audio; const char *texto; } RoteiroHank;
static const RoteiroHank roteiros[4] = {
    { "",
      "Impressionante! ..." },
    { "",
      "Muito bem..." },
    { "",
      "Parabéns! .." },
    { "",
      "Muito bom! ..." }
};

int main(void)
{
    srand((unsigned)time(NULL));

    const int screenWidth  = 1920;
    const int screenHeight = 1080;
    InitWindow(screenWidth, screenHeight, "Blindspot Undercovered – Versão Interrogatório-Only");
    InitAudioDevice();
    SetTargetFPS(800);

    Music music = LoadMusicStream("");
    SetMusicVolume(music, 0.7f);
    PlayMusicStream(music);

    SelecionarPerguntasAleatorias();
    InitPlayerStats(&playerStats);

    strcpy(gPlayerName, "Miccuci Do Borel");
    strcpy(gSelectedCharacterName, "Jade");
    playerStats.isPassed_D01 = true;
    playerStats.isPassed_D02 = false;
    playerStats.isPassed_D03 = true;
    playerStats.isPassed_D04 = true;
    playerStats.amountOfLives_D02 = 2;
    playerStats.quantityOfIcons_D04 = 37;

    bool interrogatorioInitialized = false;
    bool debugInitialized          = false;
    bool ranking_Initialized = false;
    int  perguntaAtual             = 0; 
    int  interrogatorioCount       = 0; 

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);

        if (state == INTERROGATORIO)
        {
            if (!interrogatorioInitialized)
            {
                Init_Interrogatorio(perguntaAtual,
                                    roteiros[perguntaAtual].audio,
                                    roteiros[perguntaAtual].texto);
                interrogatorioInitialized = true;
            }

            Update_Interrogatorio();
            Draw_Interrogatorio();

            if (Fase_Interrogatorio_Concluida())
            {
                Unload_Interrogatorio();
                interrogatorioInitialized = false;

                interrogatorioCount++;
                perguntaAtual++;

                if (interrogatorioCount >= 4)
                {
                    SetPlayerGeneralStats(&playerStats);
                    AppendPlayerToRankingFile(&playerStats, "ranking.txt");

                    state = APP_DEBUG;
                }
                else
                {
                }
            }
        }
        else if (state == APP_DEBUG)
        {
            if (!debugInitialized)
            {
                InitDebug();
                debugInitialized = true;
            }

            UpdateDebug();
            DrawDebug();

            if (IsKeyPressed(KEY_ENTER))
            {
                state = APP_RANKING;
            }
            

        }
        else if (state == APP_RANKING)
        {
            if (!ranking_Initialized)
            {
                Init_Ranking();
                ranking_Initialized = true;
            }
            Update_Ranking();
            Draw_Ranking();
        }
    }

    if (state == INTERROGATORIO && interrogatorioInitialized)
        Unload_Interrogatorio();
    if (state == APP_DEBUG && debugInitialized)
        UnloadDebug();

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    return 0;
}
*/