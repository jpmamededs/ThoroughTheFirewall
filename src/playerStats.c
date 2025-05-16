#include "playerStats.h"
#include <stdbool.h>
#include <string.h>
#include "ranking.h"
#include <stdlib.h>
#include <stdio.h>
#include "gemini.h"

int  notasIA[MAX_PERGUNTAS]                  = {0};
char relatoriosIA[MAX_PERGUNTAS][512]        = {0};
char relatorioGeralIA[1024]                  = {0};
char gPlayerName[MAX_PLAYER_NAME]            = {0};
char gSelectedCharacterName[MAX_PLAYER_NAME] = "";

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

static void SetPlayerAndCharacter(PlayerStats *stats, const char *playerName, const char *characterName)
{
    if (playerName == NULL || playerName[0] == '\0' || strspn(playerName, " ") == strlen(playerName))
        strncpy(stats->playerName, "Sem Nome", MAX_PLAYER_NAME - 1);
    else
        strncpy(stats->playerName, playerName, MAX_PLAYER_NAME - 1);
    stats->playerName[MAX_PLAYER_NAME - 1] = '\0';

    if (characterName) {
        strncpy(stats->characterName, characterName, MAX_PLAYER_NAME - 1);
        stats->characterName[MAX_PLAYER_NAME - 1] = '\0';
    }
}

static void GerarRelatorioGeralIA(char *relatorioGeral, size_t tamanho) 
{
    char prompt[4096];
    snprintf(prompt, sizeof(prompt),
        "Você é o agente Hank, responsável por avaliar um candidato a uma vaga de segurança cibernética no FBI.\n\n"
        "Abaixo estão os relatórios individuais da IA após cada desafio realizado pelo candidato.\n"
        "Com base nesses relatórios, crie um relatório final resumido que avalie o desempenho geral do candidato, considerando:\n"
        "- Postura ética\n"
        "- Responsabilidade\n"
        "- Uso consciente das habilidades\n"
        "Relatórios individuais:\n"
    );

    for (int i = 0; i < 4; i++) {
        char buffer[512];
        snprintf(buffer, sizeof(buffer), "Relatório %d: %s\n", i + 1, relatoriosIA[i]);
        strcat(prompt, buffer);
    }

    strcat(prompt, "\nFormato da resposta:\nRELATORIO_GERAL=<relatório consolidado e objetivo>");

    char retorno[1024] = {0};
    ObterRespostaGemini(prompt, retorno);

    const char *pRel = strstr(retorno, "RELATORIO_GERAL=");
    if (pRel) {
        strncpy(relatorioGeral, pRel + strlen("RELATORIO_GERAL="), tamanho - 1);
    }
}

static int SomarNotasIA(void) 
{
    int soma = 0;
    for (int i = 0; i < 4; i++) soma += notasIA[i];
    return soma;
}

static float CalculatePlayerScore(PlayerStats *ps)
{
    if (!ps) return 0.0f;

    float notaGeral = 0.0;

    notaGeral += ps->aiOverallScore * 0.5f;

    if (ps->isPassed_D01) notaGeral += 10.0f;
    if (ps->isPassed_D02) notaGeral += 15.0f;
    if (ps->isPassed_D03) notaGeral += 10.0f;
    if (ps->isPassed_D04) notaGeral += 15.0f;

    float bonus = 0.0;

    if (ps->isPassed_D02) bonus += (0.53 * (float)ps->amountOfLives_D02);
    if (ps->isPassed_D04) bonus += (0.2 * (float)ps->quantityOfIcons_D04);

    notaGeral += bonus;

    int desafiosAprovados =
        (ps->isPassed_D01 ? 1 : 0) +
        (ps->isPassed_D02 ? 1 : 0) +
        (ps->isPassed_D03 ? 1 : 0) +
        (ps->isPassed_D04 ? 1 : 0);

    ps->isPassouSelecao = (ps->aiOverallScore >= 60.0f && desafiosAprovados >= 3);

    return notaGeral;
}

void SetPlayerGeneralStats(PlayerStats *ps)
{
    if (!ps) return;

    SetPlayerAndCharacter(ps, gPlayerName, gSelectedCharacterName);
    ps->aiOverallScore = SomarNotasIA();
    ps->notalGeral = CalculatePlayerScore(ps);
    GerarRelatorioGeralIA(ps->relatorioGeral, sizeof(ps->relatorioGeral));
}
