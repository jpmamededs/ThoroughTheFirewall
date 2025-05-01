#include "raylib.h"
#include "cutscenes.h"
#include "menu.h"

typedef enum { APP_CUTSCENES, APP_MENU, APP_GAME } AppState;

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);

    AppState state = APP_CUTSCENES;
    InitCutscenes();

    while (!WindowShouldClose())
    {
        if (state == APP_CUTSCENES)
        {
            UpdateCutscenes();
            DrawCutscenes();
            if (CutscenesEnded())
            {
                UnloadCutscenes();
                InitMenu();
                state = APP_MENU;
            }
        }
        else if (state == APP_MENU)
        {
            UpdateMenu();
            DrawMenu();
            if (MenuStartGame())
            {
                UnloadMenu();
                state = APP_GAME;
            }
        }
        else if (state == APP_GAME)
        {
            BeginDrawing();
            ClearBackground(GREEN);

            // Agora mostra diretamente o nome do personagem escolhido!
            DrawText("Personagem escolhido:", 40, 40, 28, BLACK);
            DrawText(MenuSelectedCharacterName(), 80, 80, 40, DARKGRAY);

            EndDrawing();
        }
    }

    if (state == APP_CUTSCENES) UnloadCutscenes();
    if (state == APP_MENU) UnloadMenu();

    CloseWindow();
    return 0;
}