#include "playerStats.h"
#include <string.h>

void InitPlayerStats(PlayerStats *stats)
{
    memset(stats, 0, sizeof(PlayerStats));
}

void SetD01Result(PlayerStats *stats, bool passed, int duration)
{
    stats->isPassed_D01 = passed;
    stats->duration_D01 = duration;
}

void SetD02Result(PlayerStats *stats, bool passed, int duration, int lives)
{
    stats->isPassed_D02 = passed;
    stats->duration_D02 = duration;
    stats->amountOfLives_D02 = lives;
}

void SetD03Result(PlayerStats *stats, bool passed, int duration)
{
    stats->isPassed_D03 = passed;
    stats->duration_D03 = duration;
}

void SetD04Result(PlayerStats *stats, bool passed, int icons)
{
    stats->isPassed_D04        = passed;
    stats->quantityOfIcons_D04 = icons;
}

void SetAIOverallScore(PlayerStats *stats, int score)
{
    stats->aiOverallScore = score;
}

void SetPlayerAndCharacter(PlayerStats *stats, const char *playerName, const char *characterName)
{
    if (playerName) {
        strncpy(stats->playerName, playerName, NAME_MAX_LEN - 1);
        stats->playerName[NAME_MAX_LEN - 1] = '\0';
    }
    if (characterName) {
        strncpy(stats->characterName, characterName, NAME_MAX_LEN - 1);
        stats->characterName[NAME_MAX_LEN - 1] = '\0';
    }
}
