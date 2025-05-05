#ifndef TYPEWRITER_SYNC_H
#define TYPEWRITER_SYNC_H

#include "raylib.h"
#include "generalFunctions.h"

typedef struct {
    TypeWriter writer;
    Sound      sound;
    int        totalChars;
    float      soundDuration;
    double     startTime;
} SyncDialogue;

// Inicializa (mede duração, carrega e toca o som, grava startTime)
void InitSyncDialogue(SyncDialogue *dlg, const char *audioFilePath, const char *text);

// Atualiza cada frame (usa GetTime() para calcular progresso)
void UpdateSyncDialogue(SyncDialogue *dlg);

// Retorna true quando texto e som acabarem
bool SyncDialogueDone(const SyncDialogue *dlg);

// Descarrega o som
void UnloadSyncDialogue(SyncDialogue *dlg);

#endif // TYPEWRITER_SYNC_H
