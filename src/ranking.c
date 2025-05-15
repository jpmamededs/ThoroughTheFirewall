#include "ranking.h"
#include "raylib.h"
#include "playerStats.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

static bool fase_concluida = false; 

void Init_Ranking(void) 
{
    fase_concluida = false;
}

void Update_Ranking(void) 
{

    if (IsKeyPressed(KEY_ENTER))
    {
        fase_concluida = true;
    }
    
}

void Draw_Ranking(void) 
{
    BeginDrawing();
    ClearBackground(BLACK);

    DrawText("Ranking", 20, 20, 20, WHITE);

    EndDrawing();
}

bool Ranking_Concluido(void)
{
    return fase_concluida;
}

void Unload_Ranking(void) 
{

}
