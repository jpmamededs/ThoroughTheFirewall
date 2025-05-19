#ifndef FASE1_3_H
#define FASE1_3_H

#include "generalFunctions.h"
#include "playerStats.h"
#include <stdbool.h>

typedef struct { 
    const char *pergunta;
} Question;

typedef enum { 
    APRESENTACAO, 
    FALA_HANK, 
    TRANSICAO_PRE_PERGUNTA,
    PERGUNTA_INTERROGATORIO, 
    ETAPA_TOTAL 
} InterrogatorioStage;

typedef struct Node {
    InterrogatorioStage stage;
    struct Node *prev;
    struct Node *next;
} Node;

void Init_Interrogatorio(int perguntaIndex, const char *audio, const char *texto);
void Update_Interrogatorio(void);
void Draw_Interrogatorio(void);
void Unload_Interrogatorio(void);
void SelecionarPerguntasAleatorias(void);
bool Fase_Interrogatorio_Concluida(void);

void AddNode(InterrogatorioStage st); 
void NextStage(void);
void ClearStages(void);

#define TOTAL_PERGUNTAS 10
extern int perguntasSelecionadas[MAX_PERGUNTAS];
extern Node *atual;

#endif
