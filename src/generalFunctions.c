// generalFunctions.c
#include "generalFunctions.h"
#include "gemini.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

int notasIA[MAX_PERGUNTAS] = {0};
char relatoriosIA[MAX_PERGUNTAS][512] = {0};
char gPlayerName[MAX_PLAYER_NAME] = {0};
char relatorioGeralIA[1024] = {0};

// ====== TYPEWRITER ======
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

// ====== TYPERASER ======

void InitTypeEraser(TypeEraser* te, const char* text, float speed) {
    if (!te) return;
    te->text = text;
    te->length = strlen(text);
    te->drawnChars = te->length;  // Começa com o texto completo
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

// ===== DIALOGUE DATA ===== 
void InitDialogueQuestion(DialogueQuestion* dq, const char* pergunta_txt, DialogueOption* opcoes, int num_opcoes, float timer_total) {
    if (!dq) return;
    dq->pergunta_txt = pergunta_txt;
    dq->opcoes = opcoes;
    dq->num_opcoes = num_opcoes;
    dq->opcao_selecionada_usuario = -1;
    dq->respondeu = false;
    dq->timer_restante = timer_total;
    dq->timer_total = timer_total;
    dq->timer_ativo = true;
    dq->timer_explodiu = false;
    for (int i=0; i<num_opcoes; ++i)
        dq->opcoes[i].desabilitada = false; // reabilita tudo
}
void UpdateDialogueQuestion(DialogueQuestion* dq, float deltaTime) {
    if (!dq) return;
    if (!dq->timer_explodiu && dq->timer_ativo && !dq->respondeu) {
        dq->timer_restante -= deltaTime;
        if (dq->timer_restante <= 0.0f) {
            dq->timer_restante = 0.0f;
            dq->timer_explodiu = true;
            dq->timer_ativo = false;
            for(int i=0; i<dq->num_opcoes; ++i)
                dq->opcoes[i].desabilitada = true;
        }
    }
}
// Navegação por teclado entre opções habilitadas
void DialogueNavigateOptions(const DialogueQuestion* dq, int* selectedIndex, int direction) {
    if (!dq || !selectedIndex) return;
    int idx = *selectedIndex;
    int tries = 0;
    do {
        idx = (idx + direction + dq->num_opcoes) % dq->num_opcoes;
        tries++;
    } while (dq->opcoes[idx].desabilitada && tries < dq->num_opcoes);
    if (!dq->opcoes[idx].desabilitada)
        *selectedIndex = idx;
}
// Verifica se o usuário clicou em alguma opção (retorna índice ou -1)
int GetDialogueOptionClick(const DialogueQuestion* dq,
    int offsetX, int offsetY, int baseWidth, int rectHeight, int spacing, int larguraStep)
{
    if (!dq) return -1;
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < dq->num_opcoes; i++) {
        int rectWidth = baseWidth + i * larguraStep;
        int y = offsetY + i * (rectHeight + spacing);
        int x = GetScreenWidth() - rectWidth - offsetX;
        Rectangle rec = {x, y, rectWidth, rectHeight};
        if (!dq->opcoes[i].desabilitada &&
            CheckCollisionPointRec(mouse, rec) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return i;
        }
    }
    return -1;
}

// ===== VISUAL (Desenhar opções, timer, etc) ===== 
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
// Desenha UMA opção visualmente
void DrawDialogueOption(const DialogueOption* opt, Rectangle rec, bool selected, bool disabled, bool blink, Color base, Color txtCor) {
    Color cor = base;
    DrawRectangleRounded(rec, 0.32f, 16, cor);
    DrawText(opt->texto, rec.x+36, rec.y+rec.height/2-24/2, 24, txtCor);
}

// Desenha todas as opções, com prioridade total ao blink!
void DrawAllDialogueOptions(const DialogueQuestion* dq, int selected, int offsetX, int offsetY,
    int baseWidth, int rectHeight, int spacing, int larguraStep,
    int blinkWrong, bool blink, int blinkCorrect)
{
    Color rectGray      = (Color){56, 56, 56, 216};
    Color hoverGreen    = (Color){26,110,51,235};
    Color disabledGray  = (Color){90,90,90,175};
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < dq->num_opcoes; i++) {
        int rectWidth = baseWidth + i * larguraStep;
        int y = offsetY + i * (rectHeight + spacing);
        int x = GetScreenWidth() - rectWidth - offsetX;
        Rectangle rec = {x, y, rectWidth, rectHeight};
        int mouseOver = CheckCollisionPointRec(mouse, rec);
        bool optSelected = (selected == i);
        Color txtCor = WHITE;
        Color baseCor = rectGray;

        // === PRIORIDADE: blinkWrong > blinkCorrect > hover > desabilitado > normal ===
        if ((blinkWrong == i) && blink) {
            baseCor = (Color){230, 30, 30, 210};  // Vermelho forte
            txtCor = WHITE;
        }
        else if ((blinkCorrect == i) && blink) {
            baseCor = (Color){26, 110, 51, 255};  // Verde forte
            txtCor = WHITE;
        }
        else if (dq->opcoes[i].desabilitada) {
            baseCor = disabledGray;
            txtCor = (Color){180,180,180,210};
        }
        else if (!dq->respondeu && !dq->opcoes[i].desabilitada && (mouseOver || optSelected)) {
            baseCor = hoverGreen;
        }

        DrawDialogueOption(&dq->opcoes[i], rec, optSelected, dq->opcoes[i].desabilitada, blink, baseCor, txtCor);
    }
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

void InitPlayerName(void) {
    memset(gPlayerName, 0, sizeof(gPlayerName));
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