#ifndef RANKING_H
#define RANKING_H

#include <stdbool.h>
#include "playerStats.h"

void Init_Ranking(void);
void Update_Ranking(void);
void Draw_Ranking(void);
void Unload_Ranking(void);
bool Ranking_Concluido(void);
void AppendPlayerToRankingFile(PlayerStats *ps, char *file);

#endif