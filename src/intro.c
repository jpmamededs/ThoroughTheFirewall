#include "intro.h"
#include "generalFunctions.h"
#include "raylib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define INTRO_TEXT_SPEED 42
#define INTRO_PARTS       5
#define BOX_WIDTH_RATIO   0.75f    // largura da caixa (75 % da tela)
#define BOX_HEIGHT_RATIO  0.18f    // altura da caixa (50 % da tela)
#define TYPING_SFX_PATH  "src/music/aiSpeaking.mp3"
#define BACKGROUND_MUSIC  "src/music/soundscrate-hacker.mp3"
#define TYPING_VOLUME      0.65f

static char   *introParts[INTRO_PARTS] = {NULL};
static float   partDurations[INTRO_PARTS] = {0}; // segundos p/ cada página
static int     currentPart   = 0;
static float   partTimer     = 0;                // tempo decorrido na página
static TypeWriter partWriter;

static Font      font;
static Texture2D hackerBg;
static Texture2D agentSecreto;
static Music     typingMusic;
static Music     bgMusic;

static float fadeAlpha = 1.0f;
static bool  fadingOut = false;
#define FADE_DURATION 2.0f

static void DrawTextBoxedSafe(Font font, const char *text, Rectangle rec, int fontSize, int spacing, Color color)
{
    float x = rec.x, y = rec.y, maxW = rec.width;
    int len = strlen(text), lineLen = 0;
    char line[1024] = {0};

    for (int i = 0; i <= len; i++) {
        char c = text[i];
        line[lineLen++] = c;

        if (c == '\n' || c == '\0') {          // quebra natural
            line[lineLen - 1] = '\0';
            DrawTextEx(font, line, (Vector2){x, y}, fontSize, spacing, color);
            y += fontSize + 4;
            lineLen = 0; line[0] = '\0';
        } else {                               // quebra por largura
            line[lineLen] = '\0';
            int w = MeasureTextEx(font, line, fontSize, spacing).x;
            if (w > maxW) {
                int b = lineLen - 1;
                while (b > 0 && line[b] != ' ') b--;
                if (b == 0) b = lineLen - 1;

                char tmp = line[b];
                line[b] = '\0';
                DrawTextEx(font, line, (Vector2){x, y}, fontSize, spacing, color);
                y += fontSize + 4;

                int j = 0;
                if (tmp == ' ') b++;
                while (b < lineLen) line[j++] = line[b++];
                lineLen = j; line[lineLen] = '\0';
            }
        }
    }
}

static void StartTypingSfx(void)
{
    StopMusicStream(typingMusic);
    SeekMusicStream(typingMusic, 0);
    PlayMusicStream(typingMusic);
}
static void PauseTypingSfx(void) 
{ 
    PauseMusicStream(typingMusic); 
}

void InitIntro(const char *nomePersonagem, const float tempos[])
{
    const char *templates[INTRO_PARTS] = {
        "Você, %s, foi selecionado para integrar um seleto grupo composto pelos melhores desenvolvedores da América, convidados a trabalhar na CyberTech.Inc, a empresa mais renomada do mundo em cibersegurança financeira.\n",
        "No entanto, ao contrário dos demais colegas, seus interesses são muito mais obscuros. Seu verdadeiro objetivo não é proteger a empresa, mas sim infiltrá-la. O seu desafio será roubar todos os seus dados da CyberTech, instalando um malware sofisticado nos sistemas da companhia, agindo exclusivamente em benefício próprio.\n",
        "Essa missão, porém, está longe de ser simples. Para alcançar seu plano, será preciso superar inúmeros desafios, enfrentar situações de extremo risco e manter sua fachada durante toda sua estadia na CyberTech.Inc\n",
        "A tensão aumenta ainda mais quando o investigador cibernético mais competente do mercado, Hank Micucci, também é contratado pela empresa, determinado a descobrir qualquer sinal de traição ou vazamento interno.\n",
        "A cada decisão, seu disfarce pode ruir e será necessário travar uma batalha mental com Hank e os demais desenvolvedores para ganhar tempo para concluir seu objetivo."
    };

    // libera strings antigas
    for (int i = 0; i < INTRO_PARTS; i++) {
        if (introParts[i]) { free(introParts[i]); introParts[i] = NULL; }
    }

    // copia tempos recebidos
    for (int i = 0; i < INTRO_PARTS; i++)
        partDurations[i] = tempos ? tempos[i] : 5.0f; // default = 5 s

    // formata os textos
    for (int i = 0; i < INTRO_PARTS; i++) {
        size_t len = strlen(templates[i]) + strlen(nomePersonagem) + 16;
        introParts[i] = malloc(len);
        if (i == 0) snprintf(introParts[i], len, templates[i], nomePersonagem);
        else        strcpy(introParts[i],    templates[i]);
    }

    currentPart = 0;
    partTimer   = 0;
    InitTypeWriter(&partWriter, introParts[currentPart], INTRO_TEXT_SPEED);

    font        = GetFontDefault();
    hackerBg    = LoadTexture("src/sprites/hacker-bg.png");
    agentSecreto= LoadTexture("src/sprites/agent_secreto.png");

    // som
    typingMusic  = LoadMusicStream(TYPING_SFX_PATH);
    bgMusic = LoadMusicStream(BACKGROUND_MUSIC);

    SetMusicVolume(typingMusic, TYPING_VOLUME);
    SetMusicVolume(bgMusic, 3.5f);
    StartTypingSfx();
    PlayMusicStream(bgMusic);    
}

void UpdateIntro(void)
{
    fadeAlpha = UpdateFade(GetFrameTime(), FADE_DURATION, !fadingOut);

    float dt = GetFrameTime();

    if (IsKeyPressed(KEY_SPACE)) {
        currentPart = INTRO_PARTS - 1;
        partWriter.done = true;
        partWriter.drawnChars = partWriter.length;
        partTimer = partDurations[currentPart];
        PauseTypingSfx();
        return;
    }
    
    UpdateMusicStream(typingMusic);
    UpdateMusicStream(bgMusic);
    UpdateTypeWriter(&partWriter, dt, false);

    // se terminou de digitar, pausa som
    if (partWriter.done) PauseTypingSfx();

    partTimer += dt;

    // troca de página automaticamente
    if (partWriter.done &&
        partTimer >= partDurations[currentPart] &&
        currentPart < INTRO_PARTS - 1)
    {
        currentPart++;
        partTimer = 0;
        InitTypeWriter(&partWriter, introParts[currentPart], INTRO_TEXT_SPEED);
        StartTypingSfx(); 
    }

    if (!fadingOut && (currentPart == INTRO_PARTS - 1) && partWriter.done && partTimer >= partDurations[currentPart])
    {
        fadingOut = true;
    }
}

void DrawIntro(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();

    BeginDrawing();
    DrawTexturePro(hackerBg,
                   (Rectangle){0,0,hackerBg.width,hackerBg.height},
                   (Rectangle){0,0,w,h}, (Vector2){0,0}, 0.0f, WHITE);

    float boxW = w * BOX_WIDTH_RATIO;
    float boxH = h * BOX_HEIGHT_RATIO;
    float bottom = h * 0.05f;
    float boxX = (w - boxW) / 2.f;
    float boxY = h - bottom - boxH;

    DrawRectangleRounded((Rectangle){boxX,boxY,boxW,boxH}, 0.07f, 32,
                         (Color){0,0,0,180});

    float scale = 0.6f;
    float agentH = agentSecreto.height * scale;
    float agentX = boxX + 20;
    float agentY = boxY - agentH;
    
    DrawTextureEx(agentSecreto, (Vector2){agentX, agentY}, 0, scale, WHITE);

    int fontSize = 28, margin = 28;
    Rectangle rec = { boxX + margin, boxY + margin,
                      boxW - 2*margin, boxH - 2*margin };

    if (partWriter.drawnChars > 0) {
        char *tmp = malloc(partWriter.drawnChars + 1);
        strncpy(tmp, introParts[currentPart], partWriter.drawnChars);
        tmp[partWriter.drawnChars] = '\0';
        DrawTextBoxedSafe(font, tmp, rec, fontSize, 2, WHITE);
        free(tmp);
    }

    float progress = partTimer / partDurations[currentPart];
    if (progress > 1) progress = 1;
    DrawRectangleRec((Rectangle){
        boxX, boxY - 8,
        boxW * progress, 4
    }, (Color){255,255,255,200});

    if (fadeAlpha > 0.0f) {
        DrawRectangle(0, 0, w, h, (Color){0, 0, 0, (unsigned char)(fadeAlpha * 255)});
    }

    EndDrawing();
}

bool IntroEnded(void)
{
    return fadingOut && fadeAlpha >= 1.0f;
}

void UnloadIntro(void)
{
    for (int i = 0; i < INTRO_PARTS; i++) {
        if (introParts[i]) { free(introParts[i]); introParts[i] = NULL; }
    }
    UnloadTexture(hackerBg);
    UnloadTexture(agentSecreto);
    StopMusicStream(bgMusic);
    UnloadMusicStream(bgMusic);
}
