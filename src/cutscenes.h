#ifndef CUTSCENES_H
#define CUTSCENES_H
#include "raylib.h"
void InitCutscenes(void);
void UpdateCutscenes(void);
void DrawCutscenes(void);
bool CutscenesEnded(void);
void UnloadCutscenes(void);
#endif