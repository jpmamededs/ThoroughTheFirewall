#include "fasePCServer.h"
#include "raylib.h"

void InitFasePCServer(void) {
    // Não precisa inicializar nada!
}

void UpdateFasePCServer(void) {
    // Não há lógica para fase5 básica
}

void DrawFasePCServer(void) {
    BeginDrawing();
    ClearBackground(PURPLE);
    DrawText("FASE PC Server", GetScreenWidth()/2 - MeasureText("FASE PC Server", 64)/2, GetScreenHeight()/2 - 32, 64, YELLOW);
    EndDrawing();
}

void UnloadFasePCServer(void) {
    // Não há recursos para liberar!
}