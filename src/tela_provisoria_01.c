#include "tela_provisoria_01.h"
#include "raylib.h"
#include "generalFunctions.h"

static bool fase_concluida = false; 

void Init_Tela_01(void) 
{




    fase_concluida = false;
}

void Update_Tela_01(void) 
{



    if (IsKeyPressed(KEY_ENTER))
    {
        // Define que a tela acaba!
        fase_concluida = true;
    }
}

void Draw_Tela_01(void) 
{
    BeginDrawing();
    ClearBackground(RAYWHITE);



    int x = (400 - MeasureText("Texto Simples na Tela", 40)) / 2;
    int y = 400 / 2 - 20;
    DrawText("Texto Simples na Tela 01", x, y, 40, DARKGRAY);



    EndDrawing();
}

bool Fase_Tela01_Concluida(void)
{
    return fase_concluida;
}

void Unload_Tela_01(void) 
{
    // Lógica de desalocar
}
