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
#define PAUSE_BETWEEN_PARTS 0.5f

static char   *introParts[INTRO_PARTS] = {NULL};
static int     currentPart   = 0;

typedef enum { INTRO_TYPING, INTRO_ERASING } IntroState;
static IntroState introState = INTRO_TYPING;
static float  showDurations [INTRO_PARTS] = {0};   // visível
static float  eraseDurations[INTRO_PARTS] = {0};   // apagando
static TypeWriter partWriter;
static TypeEraser  eraser;
static float partElapsed = 0.0f;
static bool pauseAfterErase = false;

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

void InitIntro(const float temposShow[], const float temposErase[])
{
    const char *templates[INTRO_PARTS] = {
        "Você, %s, é um jovem talento da cibersegurança e foi selecionado para participar de um processo seletivo ultra secreto do FBI. \n",
        "A missão é simples: provar que você é o melhor entre os quatro candidatos, completando uma série de desafios práticos relacionados à área de cibersegurança.\n",
        "O responsável pela seleção é o Agente Hank Miccuci, um especialista rigoroso e exigente, que avaliará não só suas habilidades técnicas, mas também sua ética e responsabilidade durante as provas.\n",
        "Para conquistar a vaga, você precisará ser rápido, preciso e manter a integridade, pois qualquer atitude antiética pode custar sua chance.\n",
        "Mostre que você é capaz de defender sistemas críticos e se torne o próximo agente cibernético do FBI\n"
    };

    // libera strings antigas
    for (int i = 0; i < INTRO_PARTS; i++) {
        if (introParts[i]) { free(introParts[i]); introParts[i] = NULL; }
    }

    // copia tempos recebidos
    for (int i = 0; i < INTRO_PARTS; i++) {
        showDurations [i] = temposShow  ? temposShow [i] : 6.0f;
        eraseDurations[i] = temposErase ? temposErase[i] : 3.0f;
    }

    // formata os textos
    for (int i = 0; i < INTRO_PARTS; i++) {
        size_t len = strlen(templates[i]) + strlen(gSelectedCharacterName) + 16;
        introParts[i] = malloc(len);
        if (i == 0) snprintf(introParts[i], len, templates[i], gSelectedCharacterName);
        else        strcpy(introParts[i],    templates[i]);
    }

    currentPart = 0;
    partElapsed = 0.0f;
    introState  = INTRO_TYPING;

    InitTypeWriter(&partWriter, introParts[currentPart], INTRO_TEXT_SPEED);
    float spd0 = (float)partWriter.length / eraseDurations[currentPart];
    InitTypeEraser(&eraser, introParts[currentPart], spd0);

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
    float dt = GetFrameTime();
    fadeAlpha = UpdateFade(GetFrameTime(), FADE_DURATION, !fadingOut);

    if (pauseAfterErase) {
        partElapsed += dt;
        UpdateMusicStream(bgMusic);
        if (partElapsed >= PAUSE_BETWEEN_PARTS) {
            pauseAfterErase = false;
            InitTypeWriter(&partWriter, introParts[currentPart], INTRO_TEXT_SPEED);
            float spd = (float)partWriter.length / eraseDurations[currentPart];
            InitTypeEraser(&eraser, introParts[currentPart], spd);
            introState  = INTRO_TYPING;
            partElapsed = 0.0f;
            StartTypingSfx();
        }
        return;  // Pausa, então não faz mais nada
    }

    if (IsKeyPressed(KEY_SPACE)) {
        currentPart  = INTRO_PARTS - 1;
        introState   = INTRO_ERASING;
        partWriter.done = true;
        partElapsed  = showDurations[currentPart];   // força ir pro apagar
        StartTypingSfx();            // garante som no erase
    }
    
    UpdateMusicStream(typingMusic);
    UpdateMusicStream(bgMusic);
    partElapsed += dt;

    switch (introState) {
    case INTRO_TYPING:
        UpdateTypeWriter(&partWriter, dt, false);
        
        if (partElapsed >= showDurations[currentPart]) {
            introState = INTRO_ERASING;
            float spd  = (float)partWriter.length / eraseDurations[currentPart];
            InitTypeEraser(&eraser, introParts[currentPart], spd);
            StartTypingSfx();                    // liga som p/ apagar
        } else if (partWriter.done) {
            PauseTypingSfx();                    // pausa até chegar a hora do erase
        }
        break;

    case INTRO_ERASING:
        UpdateTypeEraser(&eraser, dt, false);
        if (eraser.done) {
            PauseTypingSfx();                    // acabou som
            currentPart++;
            if (currentPart >= INTRO_PARTS) { fadingOut = true; break; }

            pauseAfterErase = true;
            partElapsed = 0.0f;
        }
        break;
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

    /* texto */
    if (introState == INTRO_TYPING && partWriter.drawnChars > 0) {
        DrawTextBoxedSafe(font,
            TextSubtext(introParts[currentPart],0,partWriter.drawnChars),
            rec, fontSize, 2, WHITE);
    } else if (introState == INTRO_ERASING && eraser.drawnChars > 0) {
        DrawTextBoxedSafe(font,
            TextSubtext(introParts[currentPart],0,eraser.drawnChars),
            rec, fontSize, 2, WHITE);
    }

    /* barra */
    //float totalSlot = showDurations[currentPart] + eraseDurations[currentPart];
    //if (pauseAfterErase) totalSlot += PAUSE_BETWEEN_PARTS;  // inclui pausa
    
    //float prog = partElapsed / totalSlot;
    //if (pauseAfterErase) prog = 1.0f;
    //if (prog > 1) prog = 1;
    //DrawRectangleRec((Rectangle){boxX, boxY-8, boxW*prog, 4}, (Color){255,255,255,200});

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
