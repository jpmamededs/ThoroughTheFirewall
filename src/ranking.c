#include "ranking.h"
#include "raylib.h"
#include "playerStats.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static PlayerStatsDTO RankingPlayers[MAX_RANKING_PLAYERS];
static int PlayerCont = 0;
static bool fase_concluida = false; 

static void LoadRankingFromFile(const char *file)
{
    FILE *fp = fopen(file, "r");
    if (!fp) return;

    char line[256];
    while (fgets(line, sizeof(line), fp) && PlayerCont < MAX_RANKING_PLAYERS)
    {
        char playerName[MAX_PLAYER_NAME]    = {0};
        char characterName[MAX_PLAYER_NAME] = {0};
        float pontuacao = 0;
        int   passouInt = 0;

        if (sscanf(line, " %25[^,], %25[^,], %f, %d", playerName, characterName, &pontuacao, &passouInt) == 4)
        {
            bool isPassou = (passouInt != 0);

            strncpy(RankingPlayers[PlayerCont].playerName, playerName, MAX_PLAYER_NAME);
            strncpy(RankingPlayers[PlayerCont].characterName, characterName, MAX_PLAYER_NAME);
            RankingPlayers[PlayerCont].notalGeral = pontuacao;
            RankingPlayers[PlayerCont].isPassouSelecao = isPassou;

            PlayerCont++;
        }
    }

    fclose(fp);
}

static void BubbleSort(void)
{
    for (int i = 1; i <= PlayerCont; i++) {
        for (int j = 0; j < PlayerCont-1; j++) {
            if (RankingPlayers[j].notalGeral < RankingPlayers[j + 1].notalGeral) 
            {
                PlayerStatsDTO tmp     = RankingPlayers[j];
                RankingPlayers[j]      = RankingPlayers[j + 1];
                RankingPlayers[j + 1]  = tmp;
            }
        }
    }
}

void Init_Ranking(void) 
{
    fase_concluida = false;
    PlayerCont   = 0;
    LoadRankingFromFile("ranking.txt");
    BubbleSort();
}

void Update_Ranking(void) 
{

    if (IsKeyPressed(KEY_M))
    {
        fase_concluida = true;
    }
    
}

void Draw_Ranking(void) 
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("Ranking", 20, 20, 20, WHITE);

    int y = 60;
    for (int i = 0; i < PlayerCont; ++i)
    {
        char linha[256];
        snprintf(linha, sizeof(linha), "%2d) %-25s  %-12s  %.2f  %s",
                 i + 1,
                 RankingPlayers[i].playerName,
                 RankingPlayers[i].characterName,
                 RankingPlayers[i].notalGeral,
                 RankingPlayers[i].isPassouSelecao ? "Passou" : "Reprovou");

        DrawText(linha, 40, y, 18, WHITE);
        y += 22;
    }

    EndDrawing();
}

bool Ranking_Concluido(void)
{
    return fase_concluida;
}

void Unload_Ranking(void) 
{
    PlayerCont = 0;
    for (int i = 0; i < MAX_RANKING_PLAYERS; i++) memset(&RankingPlayers[i], 0, sizeof(PlayerStatsDTO));
}
