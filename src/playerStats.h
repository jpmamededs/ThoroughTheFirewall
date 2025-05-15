#ifndef PLAYER_STATS_H
#define PLAYER_STATS_H

#include <stdbool.h>

#define NAME_MAX_LEN 30

typedef struct {
    /* Desafio 01 */
    bool isPassed_D01;
    int  duration_D01;

    /* Desafio 02 */
    bool isPassed_D02;
    int  duration_D02;
    int  amountOfLives_D02;

    /* Desafio 03 */
    bool isPassed_D03;
    int  duration_D03;

    /* Desafio 04 */
    bool  isPassed_D04;
    int   quantityOfIcons_D04;

    /* Dados gerais */
    int   aiOverallScore; 
    char  playerName[NAME_MAX_LEN];
    char  characterName[NAME_MAX_LEN];
} PlayerStats;

extern PlayerStats playerStats;

void InitPlayerStats(PlayerStats *stats);
void SetD01Result(PlayerStats *stats, bool passed, int duration);
void SetD02Result(PlayerStats *stats, bool passed, int duration, int lives);
void SetD03Result(PlayerStats *stats, bool passed, int duration);
void SetD04Result(PlayerStats *stats, bool passed, int icons);
void SetAIOverallScore(PlayerStats *stats, int score);
void SetPlayerAndCharacter(PlayerStats *stats, const char *playerName, const char *characterName);

#endif
