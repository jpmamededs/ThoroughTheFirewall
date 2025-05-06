#include "fase5.h"
#include "raylib.h"

void InitFase5(void) {
    // Não precisa inicializar nada!
}

void UpdateFase5(void) {
    // Não há lógica para fase5 básica
}

void DrawFase5(void) {
    BeginDrawing();
    ClearBackground(PURPLE);
    DrawText("FASE 5", GetScreenWidth()/2 - MeasureText("FASE 5", 64)/2, GetScreenHeight()/2 - 32, 64, YELLOW);
    EndDrawing();
}

void UnloadFase5(void) {
    // Não há recursos para liberar!
}