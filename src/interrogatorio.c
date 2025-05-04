#include "generalFunctions.h"
#include "interrogatorio.h"
#include "raylib.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

static void UpdateEtapa1(float dt);
static void UpdateEtapa2(float dt);
static void UpdateEtapa3(float dt);
static void DrawEtapa1(void);
static void DrawEtapa2(void);
static void DrawEtapa3(void);
static void DrawFade(void);


typedef struct 
{
    const char *pergunta;
} Question;

typedef enum { ETAPA_1, ETAPA_2, ETAPA_3, ETAPA_TOTAL } InterrogatorioStage;

static struct
{
    // Recursos
    Texture2D background, spriteNome, spriteBustup, spriteConfiante;
    Music     interrogationMusic;
    Sound     somSurpresa, somFalaDetetive, somFalaDetetive2;

    // Controle geral
    InterrogatorioStage stage;
    float fadeWhiteAlpha;
    bool  fadeWhiteIn, fadeWhiteOut;

    // ---------- ETAPA 1 ----------
    Vector2 posNome, posBustup;
    Vector2 targetNome, targetBustup;
    float   speed, slowSpeed, tempoAposAnimacao;
    bool    bustupChegou, somSurpresaTocado;

    // ---------- ETAPA 2 ----------
    TypeWriter writer;
    bool   mostrarConfiante, dialogoFinalizado;
    bool   somFalaTocado, somFala2Tocado;

    // ---------- ETAPA 3 ----------
    char  respostaBuf[128];   // texto digitado (buffer corrente)
    int   respostaLen;        // quantos chars já digitados
    int   perguntaAtual;      // índice da pergunta corrente
    bool  aguardandoInput;    // true enquanto jogador digita
    bool  respostaConfirmada; // true assim que o jogador pressiona ENTER

} ctx;

static void (*stageUpdates[ETAPA_TOTAL])(float) = { UpdateEtapa1, UpdateEtapa2, UpdateEtapa3 };
static void (*stageDraws  [ETAPA_TOTAL])(void ) = { DrawEtapa1,  DrawEtapa2,  DrawEtapa3  };
static Question perguntas[] = {
    { "Qual era o seu cargo ontem às 16h?" },
    { "Você acessou o servidor principal na última semana?" },
    { "Pode listar três colegas que validariam sua inocência?" }
};
static const int QTD_PERGUNTAS = sizeof(perguntas)/sizeof(perguntas[0]);

void InitInterrogatorio(void)
{
    const char *texto =
        "Então, basicamente, você e... mais 3 pessoas estão sendo interrogados por crimes "
        "cibernéticos... Eu sou o investigador desse caso, e vou te fazer algumas perguntas, ok? [APERTE ENTER]";

    // Carregar recursos --------------------------------------------------
    ctx.background        = LoadTexture("src/sprites/background_outside.png");
    ctx.spriteNome        = LoadTexture("src/sprites/detetive-hank-text.png");
    ctx.spriteBustup      = LoadTexture("src/sprites/Gumshoe_OA (1).png");
    ctx.spriteConfiante   = LoadTexture("src/sprites/detective_confident.png");
    ctx.interrogationMusic= LoadMusicStream("src/music/interrogationThemeA.mp3");
    ctx.somSurpresa       = LoadSound("src/music/surprise.mp3");
    ctx.somFalaDetetive   = LoadSound("src/music/detectiveSpeaking.mp3");
    ctx.somFalaDetetive2  = LoadSound("src/music/detectiveSpeaking2.mp3");
    SetMusicVolume(ctx.interrogationMusic, 1.0f);
    PlayMusicStream(ctx.interrogationMusic);

    // Estado geral -------------------------------------------------------
    ctx.stage = ETAPA_1;
    ctx.fadeWhiteAlpha = 0.0f;
    ctx.fadeWhiteIn = ctx.fadeWhiteOut = false;

    // ETAPA 1 ------------------------------------------------------------
    ctx.posNome   = (Vector2){ GetScreenWidth(), GetScreenHeight() - ctx.spriteNome.height - 50 };
    ctx.posBustup = (Vector2){ -ctx.spriteBustup.width, GetScreenHeight() - ctx.spriteBustup.height };
    ctx.targetNome   = (Vector2){ GetScreenWidth() - ctx.spriteNome.width, ctx.posNome.y };
    ctx.targetBustup = (Vector2){ 0, ctx.posBustup.y };
    ctx.speed = 2500.f; ctx.slowSpeed = 10.f;
    ctx.tempoAposAnimacao = 0.f;
    ctx.bustupChegou = ctx.somSurpresaTocado = false;

    // ETAPA 2 ------------------------------------------------------------
    InitTypeWriter(&ctx.writer, texto, strlen(texto)/10.f);
    ctx.mostrarConfiante = ctx.dialogoFinalizado = false;
    ctx.somFalaTocado = ctx.somFala2Tocado = false;
}

void UpdateInterrogatorio(void)
{
    UpdateMusicStream(ctx.interrogationMusic);
    float dt = GetFrameTime();
    // Despacho dinâmico
    stageUpdates[ctx.stage](dt);
}

void DrawInterrogatorio(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle src = {0,0,(float)ctx.background.width,(float)ctx.background.height};
    Rectangle dst = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
    DrawTexturePro(ctx.background, src, dst, (Vector2){0,0}, 0, WHITE);

    stageDraws[ctx.stage]();   // Chama o draw da etapa atual
    DrawFade();

    EndDrawing();
}

void UnloadInterrogatorio(void)
{
    UnloadTexture(ctx.background);
    UnloadTexture(ctx.spriteNome);
    UnloadTexture(ctx.spriteBustup);
    UnloadTexture(ctx.spriteConfiante);
    UnloadMusicStream(ctx.interrogationMusic);
    UnloadSound(ctx.somSurpresa);
    UnloadSound(ctx.somFalaDetetive);
    UnloadSound(ctx.somFalaDetetive2);
}

static void UpdateEtapa1(float dt)
{
    if (!ctx.somSurpresaTocado) { PlaySound(ctx.somSurpresa); ctx.somSurpresaTocado = true; }

    if (ctx.posNome.x > ctx.targetNome.x) {
        ctx.posNome.x -= ctx.speed * dt;
        if (ctx.posNome.x < ctx.targetNome.x) ctx.posNome.x = ctx.targetNome.x;
    }
    if (!ctx.bustupChegou) {
        ctx.posBustup.x += ctx.speed * dt;
        if (ctx.posBustup.x >= ctx.targetBustup.x) {
            ctx.posBustup.x = ctx.targetBustup.x; ctx.bustupChegou = true;
        }
    }
    if (ctx.bustupChegou && ctx.posNome.x == ctx.targetNome.x) {
        ctx.posBustup.x += ctx.slowSpeed * dt;
        ctx.tempoAposAnimacao += dt;
        if (ctx.tempoAposAnimacao > 2.f && !ctx.fadeWhiteIn && !ctx.fadeWhiteOut) ctx.fadeWhiteIn = true;
    }

    if (ctx.fadeWhiteIn) {
        ctx.fadeWhiteAlpha += dt * 1.5f;
        if (ctx.fadeWhiteAlpha >= 1.f) {
            ctx.fadeWhiteAlpha = 1.f; ctx.fadeWhiteIn = false; ctx.fadeWhiteOut = true;
            ctx.stage = ETAPA_2;
            ctx.mostrarConfiante = true;
        }
    }
}

static void UpdateEtapa2(float dt)
{
    if (ctx.fadeWhiteOut) {
        ctx.fadeWhiteAlpha -= dt * 1.5f;
        if (ctx.fadeWhiteAlpha <= 0.f) { ctx.fadeWhiteAlpha = 0.f; ctx.fadeWhiteOut = false; }
    }

    if (ctx.mostrarConfiante && !ctx.somFalaTocado) {
        PlaySound(ctx.somFalaDetetive); ctx.somFalaTocado = true;
    }

    bool skip = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_SPACE);
    UpdateTypeWriter(&ctx.writer, dt, skip);

    if (ctx.writer.done && skip && !ctx.dialogoFinalizado) ctx.dialogoFinalizado = true;
    if (ctx.dialogoFinalizado && !ctx.somFala2Tocado) { PlaySound(ctx.somFalaDetetive2); ctx.somFala2Tocado = true; }

    if (ctx.dialogoFinalizado && ctx.somFala2Tocado && IsKeyPressed(KEY_ENTER)) {
        ctx.stage = ETAPA_3;
        ctx.perguntaAtual = 0;
        ctx.respostaLen = 0;
        ctx.respostaBuf[0] = '\0';
        ctx.aguardandoInput = true;
    }
}

static void UpdateEtapa3(float dt)
{
    if (!ctx.aguardandoInput) return;

    int ch;
    while ((ch = GetCharPressed()) > 0) {
        if (ctx.respostaLen < (int)sizeof(ctx.respostaBuf)-1 && ch >= 32 && ch <= 126) {
            ctx.respostaBuf[ctx.respostaLen++] = (char)ch;
            ctx.respostaBuf[ctx.respostaLen]   = '\0';
        }
    }

    if (IsKeyPressed(KEY_BACKSPACE) && ctx.respostaLen > 0) {
        ctx.respostaBuf[--ctx.respostaLen] = '\0';
    }

    if (IsKeyPressed(KEY_ENTER)) {
        ctx.respostaConfirmada = true;
        ctx.aguardandoInput = false;

        // TODO: armazenar resposta numa lista/vetor para uso posterior
        // exemplo rudimentar:
        printf("Pergunta %d -> %s\n", ctx.perguntaAtual, ctx.respostaBuf);

        // Avança ou termina
        ctx.perguntaAtual++;
        if (ctx.perguntaAtual < QTD_PERGUNTAS) {
            // prepara próxima questão
            ctx.respostaLen = 0;
            ctx.respostaBuf[0] = '\0';
            ctx.aguardandoInput = true;
        } else {
            // TOD O: prosseguir para ETAPA_4 (enviar à IA) ou sair da cutscene
        }
    }
}


static void DrawEtapa1(void)
{
    DrawTexture(ctx.spriteNome,   ctx.posNome.x,   ctx.posNome.y,   WHITE);
    DrawTexture(ctx.spriteBustup, ctx.posBustup.x, ctx.posBustup.y, WHITE);
}

static void DrawEtapa2(void)
{
    // Sprite confiante
    float scale = 1.3f;
    Rectangle src = ctx.dialogoFinalizado
                      ? (Rectangle){3029, 3357, 631, 725}
                      : (Rectangle){2087,    0, 631, 722};
    float w = src.width*scale, h = src.height*scale;
    Vector2 pos = { (GetScreenWidth()-w)/2.f, GetScreenHeight()-h };
    DrawTexturePro(ctx.spriteConfiante, src, (Rectangle){pos.x,pos.y,w,h}, (Vector2){0,0},0, WHITE);

    // Caixa de diálogo
    if (!ctx.dialogoFinalizado) {
        const float boxH=120.f;
        DrawRectangle(50, GetScreenHeight()-boxH-30, GetScreenWidth()-100, boxH, (Color){0,0,0,200});
        DrawRectangleLines(50, GetScreenHeight()-boxH-30, GetScreenWidth()-100, boxH, WHITE);

        char buf[512]={0}; strncpy(buf, ctx.writer.text, ctx.writer.drawnChars);
        const int fs=24, maxW=GetScreenWidth()-140, x0=70, lineH=fs+6;
        int y=GetScreenHeight()-boxH-10;
        const char* line=buf;
        while(*line){
            const char* p=line; int lastSp=-1,c=0,w=0;
            while(*p && w<maxW){ if(*p==' ') lastSp=c; ++p;++c; char tmp[256]={0}; strncpy(tmp,line,c); w=MeasureText(tmp,fs);}
            if(w>=maxW&&lastSp>=0) c=lastSp;
            char out[256]={0}; strncpy(out,line,c); DrawText(out,x0,y,fs,WHITE);
            line+=c; while(*line==' ') ++line; y+=lineH;
        }
    }
}

static void DrawEtapa3(void)
{
    float scale = 1.3f;
    Rectangle src = { 3029, 3357, 631, 725 };
    float w = src.width*scale, h = src.height*scale;
    Vector2 pos = { (GetScreenWidth()-w)/2.f, GetScreenHeight()-h };
    DrawTexturePro(ctx.spriteConfiante, src,
                   (Rectangle){pos.x,pos.y,w,h}, (Vector2){0,0},0, WHITE);

    // Caixa da pergunta
    const float boxH = 160.f;
    const float boxOffsetY = 60.f;  // Quanto maior, mais alto sobe
    float boxY = GetScreenHeight() - boxH - boxOffsetY;
    DrawRectangle(50, boxY, GetScreenWidth() - 100, boxH, (Color){0,0,0,220});
    DrawRectangleLines(50, boxY, GetScreenWidth() - 100, boxH, WHITE);

    // Pergunta atual
    DrawText(perguntas[ctx.perguntaAtual].pergunta, 70, boxY + 10, 24, WHITE);

    // Campo de input (sub‑caixa)
    const float inputH = 40.f;
    int yInput = boxY + boxH - inputH - 15;
    DrawRectangle(70, yInput, GetScreenWidth()-140, inputH, (Color){20,20,20,255});
    DrawRectangleLines(70, yInput, GetScreenWidth()-140, inputH, WHITE);
    DrawText(ctx.respostaBuf, 80, yInput+8, 24, GREEN);

    // Texto digitado
    DrawText(ctx.respostaBuf, 80, yInput+8, 24, GREEN);
    if (ctx.aguardandoInput && (GetTime()*2.0 - floor(GetTime()*2.0)) > 0.5)
        DrawText("|", 80 + MeasureText(ctx.respostaBuf, 24), yInput+8, 24, GREEN);
}

static void DrawFade(void)
{
    if (ctx.fadeWhiteAlpha>0.f)
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),
                      (Color){255,255,255,(unsigned char)(ctx.fadeWhiteAlpha*255)});
}
