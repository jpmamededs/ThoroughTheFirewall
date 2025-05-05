#include "typewriter_sync.h"
#include <string.h>

void InitSyncDialogue(SyncDialogue *dlg, const char *audioFilePath, const char *text)
{
    // Texto
    dlg->totalChars = strlen(text);
    InitTypeWriter(&dlg->writer, text, 0.f);  // velocidade não importa agora

    // Duração do áudio
    Wave wave = LoadWave(audioFilePath);
    dlg->soundDuration = (float)wave.frameCount / (float)wave.sampleRate;
    UnloadWave(wave);

    // Carrega e toca
    dlg->sound = LoadSound(audioFilePath);
    PlaySound(dlg->sound);

    // Marca hora de início
    dlg->startTime = GetTime();
}

void UpdateSyncDialogue(SyncDialogue *dlg)
{
    // Quanto tempo já passou desde o start
    float played = (float)(GetTime() - dlg->startTime);
    if (played > dlg->soundDuration) played = dlg->soundDuration;

    // Quantos chars mostrar
    int charsToShow = (int)(dlg->totalChars * (played / dlg->soundDuration));
    if (charsToShow > dlg->totalChars) charsToShow = dlg->totalChars;

    dlg->writer.drawnChars = charsToShow;
    dlg->writer.done      = (charsToShow == dlg->totalChars);
}

bool SyncDialogueDone(const SyncDialogue *dlg)
{
    // Só texto já basta, pois o cálculo usa soundDuration
    return dlg->writer.done;
}

void UnloadSyncDialogue(SyncDialogue *dlg)
{
    UnloadSound(dlg->sound);
}
