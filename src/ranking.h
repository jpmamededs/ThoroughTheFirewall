#ifndef RANKING_H
#define RANKING_H

#include <stdbool.h>

#define MAX_RANKING_PLAYERS 200

void Init_Ranking(void);
void Update_Ranking(void);
void Draw_Ranking(void);
void Unload_Ranking(void);
bool Ranking_Concluido(void);

#endif