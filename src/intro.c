#include "intro.h"
#include "generalFunctions.h"
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define INTRO_TEXT_SPEED 42 // Letras por segundo

static char *introText = NULL; // Buffer realocável para o texto personalizado
static TypeWriter introWriter; // Gerencia o efeito de digitação
static Font font;

// Word wrap seguro para Raylib usando DrawTextEx
static void DrawTextBoxedSafe(Font font, const char *text, Rectangle rec, int fontSize, int spacing, Color color)
{
    float x = rec.x;
    float y = rec.y;
    float maxWidth = rec.width;
    int len = strlen(text);
    char line[1024] = {0};
    int lineLen = 0;
    for (int i = 0; i <= len; i++)
    {
        char c = text[i];
        line[lineLen++] = c;
        if (c == '\n' || c == '\0')
        {
            line[lineLen - 1] = '\0';
            DrawTextEx(font, line, (Vector2){x, y}, fontSize, spacing, color);
            y += fontSize + 4;
            lineLen = 0;
            line[0] = '\0';
        }
        else
        {
            // Verifica largura da linha
            line[lineLen] = '\0';
            int width = MeasureTextEx(font, line, fontSize, spacing).x;
            if (width > maxWidth)
            {
                // Quebra a linha antes da última palavra
                int breakPos = lineLen - 1;
                while (breakPos > 0 && line[breakPos] != ' ')
                    breakPos--;
                if (breakPos == 0)
                    breakPos = lineLen - 1;
                char temp = line[breakPos];
                line[breakPos] = '\0';
                DrawTextEx(font, line, (Vector2){x, y}, fontSize, spacing, color);
                y += fontSize + 4;
                // Copia a palavra restante para o início da linha
                int j = 0;
                if (temp == ' ')
                    breakPos++;
                while (breakPos < lineLen)
                {
                    line[j++] = line[breakPos++];
                }
                lineLen = j;
                line[lineLen] = '\0';
            }
        }
    }
}

void InitIntro(const char *nomePersonagem)
{
    const char *baseText =
        "Você, %s, foi selecionado para integrar um seleto grupo composto pelos melhores desenvolvedores da América, convidados a trabalhar na CyberTech.Inc, a empresa mais renomada do mundo em cibersegurança financeira.\n\n"
        "No entanto, ao contrário dos demais colegas, seus interesses são muito mais obscuros. Seu verdadeiro objetivo não é proteger a empresa, mas sim infiltrá-la. O desafio da sua vida será roubar todos os seus dados da CyberTech, instalando um malware sofisticado nos sistemas da companhia, agindo exclusivamente em benefício próprio.\n\n"
        "Essa missão, porém, está longe de ser simples. Para alcançar seu plano, será preciso superar inúmeros desafios, enfrentar situações de extremo risco e manter sua fachada durante toda sua estadia na CyberTech.Inc. A tensão aumenta ainda mais quando o investigador cibernético mais competente do mercado, Hank Micucci, também é contratado pela empresa, determinado a descobrir qualquer sinal de traição ou vazamento interno.\n\n"
        "A cada decisão, seu disfarce pode ruir e será necessário travar uma batalha mental com Hank e os demais desenvolvedores para ganhar tempo para concluir seu objetivo.";

    if (introText)
        free(introText); // Libera anterior, se existir
    int tamMax = strlen(baseText) + strlen(nomePersonagem) + 64;
    introText = malloc(tamMax);
    snprintf(introText, tamMax, baseText, nomePersonagem);
    // Inicializa TypeWriter
    InitTypeWriter(&introWriter, introText, INTRO_TEXT_SPEED);
    font = GetFontDefault();
}

void UpdateIntro(void)
{
    bool skip = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_ENTER) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    UpdateTypeWriter(&introWriter, GetFrameTime(), skip);
}

void DrawIntro(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    BeginDrawing();
    ClearBackground(BLACK);

    float boxW = w * 0.80f;
    float boxH = h * 0.66f;
    float boxX = (w - boxW) / 2;
    float boxY = (h - boxH) / 2;
    Color boxFill = (Color){255, 255, 255, 60};    // Preenchimento bem transparente
    Color boxBorder = (Color){200, 200, 200, 220}; // Borda cinza clara mais opaca
    float roundness = 0.07f;
    int segments = 32;
    float borderThickness = 2.0f;

    DrawRectangleRounded((Rectangle){boxX, boxY, boxW, boxH}, roundness, segments, boxFill);
    DrawRectangleRoundedLines((Rectangle){boxX, boxY, boxW, boxH}, roundness, segments, borderThickness, boxBorder);

    int fontSize = 28;
    int margin = 48;
    float textX = boxX + margin;
    float textY = boxY + margin;
    float textW = boxW - 2 * margin;
    float textH = boxH - 2 * margin;
    Rectangle rec = {textX, textY, textW, textH};

    if (introText && introWriter.drawnChars > 0)
    {
        char *buffer = malloc(introWriter.drawnChars + 1);
        strncpy(buffer, introText, introWriter.drawnChars);
        buffer[introWriter.drawnChars] = '\0';
        DrawTextBoxedSafe(font, buffer, rec, fontSize, 2, WHITE);
        free(buffer);
    }
    if (introWriter.done && introWriter.drawnChars >= introWriter.length)
    {
        const char *msg = "[Pressione ENTER para continuar]";
        int pressFont = 22;
        int msgW = MeasureText(msg, pressFont);
        DrawText(msg, w / 2 - msgW / 2, boxY + boxH - margin - 12, pressFont, WHITE);
    }
    EndDrawing();
}

bool IntroEnded(void)
{
    return (introWriter.done && introWriter.drawnChars >= introWriter.length &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)));
}

void UnloadIntro(void)
{
    if (introText)
    {
        free(introText);
        introText = NULL;
    }
}
