#include "generalFunctions.h"
#include "gemini.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

int notasIA[MAX_PERGUNTAS] = {0};
char relatoriosIA[MAX_PERGUNTAS][512] = {0};
char gPlayerName[MAX_PLAYER_NAME] = {0};
char gSelectedCharacterName[MAX_PLAYER_NAME] = "";
char relatorioGeralIA[1024] = {0};

void InitTypeWriter(TypeWriter* tw, const char* text, float speed) {
    if (!tw) return;
    tw->text = text;
    tw->length = strlen(text);
    tw->drawnChars = 0;
    tw->accum = 0.0f;
    tw->speed = speed;
    tw->done = false;
}

void UpdateTypeWriter(TypeWriter* tw, float deltaTime, bool skip) {
    if (!tw) return;
    if (tw->drawnChars < tw->length && !tw->done) {
        tw->accum += deltaTime * tw->speed;
        int nextLen = (int)tw->accum;
        if (nextLen > tw->drawnChars) {
            tw->drawnChars = nextLen;
            if (tw->drawnChars > tw->length)
                tw->drawnChars = tw->length;
        }
        if (skip) {
            tw->drawnChars = tw->length;
            tw->done = true;
        }
    } else {
        tw->done = true;
    }
}

void SkipTypeWriter(TypeWriter* tw) {
    if (!tw) return;
    tw->drawnChars = tw->length;
    tw->done = true;
}

void InitTypeEraser(TypeEraser* te, const char* text, float speed) {
    if (!te) return;
    te->text = text;
    te->length = strlen(text);
    te->drawnChars = te->length;
    te->accum = 0.0f;
    te->speed = speed;
    te->done = false;
}

void UpdateTypeEraser(TypeEraser* te, float deltaTime, bool skip) {
    if (!te) return;
    if (te->drawnChars > 0 && !te->done) {
        te->accum += deltaTime * te->speed;
        int nextLen = te->length - (int)te->accum;
        if (nextLen < te->drawnChars) {
            te->drawnChars = nextLen;
            if (te->drawnChars < 0)
                te->drawnChars = 0;
        }
        if (skip) {
            te->drawnChars = 0;
            te->done = true;
        }
    } else {
        te->done = true;
    }
}

void SkipTypeEraser(TypeEraser* te) {
    if (!te) return;
    te->drawnChars = 0;
    te->done = true;
}

void DrawChronometer(float timer_restante, float timer_total, int x, int y, int radius) {
    DrawCircle(x, y, radius, (Color){25,25,30, 220});
    DrawCircleSectorLines((Vector2){x, y}, radius, 0, 360, 32, (Color){180,180,180,220});
    float angle = (timer_restante / timer_total) * 360.0f;
    Color barColor = (timer_restante <= 10.0f) ? (Color){230,30,30,150} : (Color){80, 230, 80, 130};
    DrawCircleSector((Vector2){x, y}, radius-4, -90, -90+angle, 60, barColor);
    char timerText[8];
    int t = (int)timer_restante;
    snprintf(timerText, sizeof(timerText), "%02d", t);
    int fonte = 49;
    Color timeColor = (timer_restante <= 5.0f) ? RED : ((timer_restante <= 10.0f) ? (Color){230,30,30,150} : WHITE);
    if (timer_restante <= 5.0f) { fonte = 58; }
    int timerTextWidth = MeasureText(timerText, fonte);
    DrawText(timerText, x - timerTextWidth/2, y - fonte/2, fonte, timeColor);
    DrawText("s", x + timerTextWidth/2 + 2, y - fonte/3 + 10, 28, (Color){170,170,210,210});
}

float UpdateFade(float dt, float duration, bool fadeIn)
{
    static float alpha = 1.0f;
    float delta = dt / duration;

    if (fadeIn) {
        alpha -= delta;
        if (alpha < 0.0f) alpha = 0.0f;
    } else {
        alpha += delta;
        if (alpha > 1.0f) alpha = 1.0f;
    }

    return alpha;
}

void DrawDica(float posX, float posY, const char *text)
{
    int dicaWidth = 420;
    int dicaHeight = 50;
    int iconSize = 20;
    int padding = 15;

    DrawRectangleRounded((Rectangle){posX, posY, dicaWidth, dicaHeight}, 0.3f, 12, (Color){30, 30, 30, 200});
    // VOLTAR DEPOIS
    //DrawRectangleRoundedLines((Rectangle){posX, posY, dicaWidth, dicaHeight}, 0.3f, 12, (Color){255, 255, 255, 100});

    int iconPosX = posX + padding;
    int iconPosY = posY + dicaHeight / 2;

    DrawCircle(iconPosX, iconPosY, iconSize / 2, (Color){100, 100, 255, 200});
    DrawText("i", iconPosX, iconPosY - 10, 20, WHITE);

    int fontSize = 18;
    int textPosX = iconPosX + iconSize + padding;
    int textPosY = posY + dicaHeight / 2 - fontSize / 2;
    int textWidth = MeasureText(text, fontSize);

    if (textWidth > dicaWidth - (2 * padding + iconSize))
    {
        fontSize = 16;
        textWidth = MeasureText(text, fontSize);
    }

    DrawText(text, textPosX, textPosY, fontSize, WHITE);
}

int SomarNotasIA(void) {
    int soma = 0;
    for (int i = 0; i < 4; i++) {
        soma += notasIA[i];
    }
    return soma;
}

void GerarRelatorioGeralIA(char *relatorioGeral, size_t tamanho) {

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