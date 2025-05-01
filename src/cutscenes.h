#ifndef CUTSCENES_H
#define CUTSCENES_H
#include "raylib.h"
void InitCutscenes(void);
void UpdateCutscenes(void);
void DrawCutscenes(void);
// retorna se terminou as cutscenes
bool CutscenesEnded(void);
void UnloadCutscenes(void);
#endif