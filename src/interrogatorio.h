#ifndef FASE1_3_H
#define FASE1_3_H

#include "generalFunctions.h"

typedef struct { 
    const char *pergunta;
} Question;

typedef enum { 
    APRESENTACAO, 
    FALA_HANK, 
    PERGUNTA_INTERROGATORIO, 
    ETAPA_TOTAL 
} InterrogatorioStage;

void InitInterrogatorio(int perguntaIndex, const char *audio, const char *texto);
void UpdateInterrogatorio(void);
void DrawInterrogatorio(void);
void UnloadInterrogatorio(void);
void SelecionarPerguntasAleatorias(void);

#define TOTAL_PERGUNTAS 10

extern int perguntasSelecionadas[MAX_PERGUNTAS];
extern bool interrogatorioFinalizado;

#endif
