#include "raylib.h"
#include "cutscenes.h"

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);
    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);

    InitCutscenes();

    while (!WindowShouldClose())
    {
        UpdateCutscenes();
        DrawCutscenes();
    }

    UnloadCutscenes();
    CloseWindow();

    return 0;
}