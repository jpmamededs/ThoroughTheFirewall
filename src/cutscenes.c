#include "raylib.h"
#include "cutscenes.h"
#include <math.h>

#define NUM_COSTAS 4

#ifndef Clamp
static inline float Clamp(float value, float min, float max)
{
    if (value < min) return min;
    if (value > max) return max;
    return value;
}
#endif

// --- Variáveis globais de cutscene ---
static Texture2D empresa1, empresa2, empresa3, empresa4;
static Texture2D detetiveNaEmpresa4;
static Texture2D minhaTinta;
static Texture2D meuSelo;
static Texture2D bg1;
static Texture2D bg2; // BG2 NOVO!
static Texture2D personagemHank;
static Texture2D v1_hank;
static Texture2D v2_hank;
static Texture2D v3_hank;
static Texture2D v4_hank;
static Texture2D nomeHank;
static Texture2D personagemAlice;
static Texture2D nomeAlice;
static Texture2D v1_alice;
static Texture2D v2_alice;
static Texture2D v3_alice;
static Texture2D bg3;
static Texture2D bg4;
static Texture2D personagemDante;
static Texture2D v1_dante;
static Texture2D v2_dante;
static Texture2D v3_dante;
static Texture2D nomeDante;
static Texture2D bg5;
static Texture2D personagemJade;
static Texture2D v1_jade;
static Texture2D v2_jade;
static Texture2D v3_jade;
static Texture2D nomeJade;
static Texture2D bgHacker;
static Texture2D costasJade, costasLevi, costasAlice, costasDante;
static Texture2D bgSemiFinal;
static Texture2D aliceComemora;
static Texture2D fotosTodos;
static Texture2D rejDante;
static Texture2D rejJade;
static Texture2D rejAlice;
static Texture2D rejLevi;
static Texture2D bgFinal;
static Texture2D logo;

static bool hackerTransicaoIniciou = false;
static bool hackerTransicaoTerminou = false;
static float hackerTransicaoStartTime = 0.0f;
static float hackerMomentoTotal = 0.0f;

static int screenWidth, screenHeight;
static float startTime = 0.0f;
static bool ended = false;


// --- Estados da animação de piscada final após rejeição --- 
static bool animPiscadasComecou = false;
static float animPiscadasStartTime = 0.0f;
static int estadoPiscadaAtual = 0; // 0 = não começou, 1=blink1, 2=blink2, 3=blink3, 4=acabou
static bool mostrandoBgFinal = false;
// Transição pincelada
static bool pinceladaIniciou = false;
static float pinceisStartTime = 0.0f;
static bool pinceladaTerminou = false;
static float momentoBG1Inteiro = 0.0f; // Quando o bg1 ficou completamente visível

// --- MOD: transição BG2 ---
static bool bg2TransicaoIniciou = false;
static float bg2TransicaoStartTime = 0.0f;
static bool bg2TransicaoTerminou = false;
static float bg2MomentoTotal = 0.0f;

static bool bg3TransicaoIniciou = false;
static float bg3TransicaoStartTime = 0.0f;
static bool bg3TransicaoTerminou = false;
static float bg3MomentoTotal = 0.0f;

static bool aliceApareceu = false;
static float aliceStartTime = 0.0f;
static bool aliceAnimTerminou = false;
static float aliceAnimTerminouTime = 0.0f;

static Texture2D personagemLevi;
static Texture2D v1_levi;
static Texture2D v2_levi;
static Texture2D v3_levi;
static Texture2D nomeLevi;
static bool aguardandoFimBgFinal = false;
static bool bg4TransicaoIniciou = false;
static float bg4TransicaoStartTime = 0.0f;
static bool bg4TransicaoTerminou = false;
static float bg4MomentoTotal = 0.0f;
static bool danteApareceu = false;
static float danteStartTime = 0.0f;
static bool danteAnimTerminou = false;
static float danteAnimTerminouTime = 0.0f;
static bool animZoomFinalIniciou = false;
static float animZoomFinalStart = 0.0f;
static bool animZoomFinalTerminou = false;

static bool bg5TransicaoIniciou = false;
static float bg5TransicaoStartTime = 0.0f;
static bool bg5TransicaoTerminou = false;
static float bg5MomentoTotal = 0.0f;
static bool jadeApareceu = false;
static float jadeStartTime = 0.0f;
static bool jadeAnimTerminou = false;
static float jadeAnimTerminouTime = 0.0f;

static bool semiFinalIniciou = false;
static float semiFinalStartTime = 0.0f;
static bool aliceComemoraAnimTerminou = false;
static float aliceComemoraTerminoTime = 0.0f;
static bool fotosTodosApareceu = false;

// --- PROTÓTIPO da transição BG2 ---
static void DrawAnimacaoBarrasBg2(float elapsed, int w, int h);

void InitCutscenes(void)
{
    screenWidth = GetMonitorWidth(0);
    screenHeight = GetMonitorHeight(0);
    personagemAlice  = LoadTexture("src/sprites/intro/personagemAlice.png");
    v1_alice        = LoadTexture("src/sprites/intro/v1_alice.png");
    v2_alice        = LoadTexture("src/sprites/intro/v2_alice.png");
    v3_alice = LoadTexture("src/sprites/intro/v3_alice.png");
    logo = LoadTexture("src/sprites/intro/logo.png");
    bgFinal = LoadTexture("src/sprites/intro/bgFinal.png");
    nomeAlice = LoadTexture("src/sprites/intro/nomeAlice.png");
    empresa1        = LoadTexture("src/sprites/intro/empresa1.jpg");
    empresa2        = LoadTexture("src/sprites/intro/empresa2.jpg");
    empresa3        = LoadTexture("src/sprites/intro/empresa3.jpg");
    empresa4        = LoadTexture("src/sprites/intro/empresa4.jpg");
    detetiveNaEmpresa4 = LoadTexture("src/sprites/intro/detetiveNaEmpresa4.png");
    minhaTinta      = LoadTexture("src/sprites/intro/tinta.png");
    meuSelo         = LoadTexture("src/sprites/intro/selo.png");
    bg1             = LoadTexture("src/sprites/intro/bg1.png");
    bg2             = LoadTexture("src/sprites/intro/bg2.png"); // NOVA!
    personagemHank  = LoadTexture("src/sprites/intro/personagemHank.png");
    v1_hank         = LoadTexture("src/sprites/intro/v1_hank.png");
    v2_hank         = LoadTexture("src/sprites/intro/v2_hank.png");
    v3_hank         = LoadTexture("src/sprites/intro/v3_hank.png");
    v4_hank         = LoadTexture("src/sprites/intro/v4_hank.png");
    nomeHank        = LoadTexture("src/sprites/intro/nomeHank.png");
    bg3 = LoadTexture("src/sprites/intro/bg3.png");
    personagemLevi  = LoadTexture("src/sprites/intro/personagemLevi.png");
    v1_levi         = LoadTexture("src/sprites/intro/v1_levi.png");
    v2_levi         = LoadTexture("src/sprites/intro/v2_levi.png");
    v3_levi         = LoadTexture("src/sprites/intro/v3_levi.png");
    nomeLevi        = LoadTexture("src/sprites/intro/nomeLevi.png");
    bg4 = LoadTexture("src/sprites/intro/bg4.png");
    personagemDante = LoadTexture("src/sprites/intro/personagemDante.png");
    v1_dante = LoadTexture("src/sprites/intro/v1_dante.png");
    v2_dante = LoadTexture("src/sprites/intro/v2_dante.png");
    v3_dante = LoadTexture("src/sprites/intro/v3_dante.png");
    nomeDante = LoadTexture("src/sprites/intro/nomeDante.png");
    bg5 = LoadTexture("src/sprites/intro/bg5.png");
    personagemJade = LoadTexture("src/sprites/intro/personagemJade.png");
    v1_jade = LoadTexture("src/sprites/intro/v1_jade.png");
    v2_jade = LoadTexture("src/sprites/intro/v2_jade.png");
    v3_jade = LoadTexture("src/sprites/intro/v3_jade.png");
    nomeJade = LoadTexture("src/sprites/intro/nomeJade.png");
    bgHacker = LoadTexture("src/sprites/intro/bgHacker.png");
    costasJade  = LoadTexture("src/sprites/intro/costasJade.png");
    costasLevi  = LoadTexture("src/sprites/intro/costasLevi.png");
    costasAlice = LoadTexture("src/sprites/intro/costasAlice.png");
    costasDante = LoadTexture("src/sprites/intro/costasDante.png");
    bgSemiFinal     = LoadTexture("src/sprites/intro/bgSemiFinal.png");
    aliceComemora   = LoadTexture("src/sprites/intro/aliceComemora.png");
    fotosTodos      = LoadTexture("src/sprites/intro/fotosTodos.png");
    rejDante = LoadTexture("src/sprites/intro/rejDante.png");
    rejJade = LoadTexture("src/sprites/intro/rejJade.png");
    rejAlice = LoadTexture("src/sprites/intro/rejAlice.png");
    rejLevi = LoadTexture("src/sprites/intro/rejLevi.png");
    
    startTime = GetTime();
    ended = false;
    pinceladaIniciou = false;
    pinceisStartTime = 0.0f;
    pinceladaTerminou = false;
    momentoBG1Inteiro = 0.0f;
    // --- BG2 transição
    bg2TransicaoIniciou = false;
    bg2TransicaoStartTime = 0.0f;
    bg2TransicaoTerminou = false;
    bg2MomentoTotal = 0.0f;

    aliceApareceu = false;
    aliceStartTime = 0.0f;
    aliceAnimTerminou = false;
    aliceAnimTerminouTime = 0.0f;

    bg3TransicaoIniciou = false;
    bg3TransicaoStartTime = 0.0f;
    bg3TransicaoTerminou = false;
    bg3MomentoTotal = 0.0f;

    bg4TransicaoIniciou = false;
    bg4TransicaoStartTime = 0.0f;
    bg4TransicaoTerminou = false;
    bg4MomentoTotal = 0.0f;
    danteApareceu = false;
    danteStartTime = 0.0f;
    danteAnimTerminou = false;
    danteAnimTerminouTime = 0.0f;

    bg5TransicaoIniciou = false;
    bg5TransicaoStartTime = 0.0f;
    bg5TransicaoTerminou = false;
    bg5MomentoTotal = 0.0f;
    jadeApareceu = false;
    jadeStartTime = 0.0f;
    jadeAnimTerminou = false;
    jadeAnimTerminouTime = 0.0f;

    hackerTransicaoIniciou = false;
    hackerTransicaoTerminou = false;
    hackerTransicaoStartTime = 0.0f;
    hackerMomentoTotal = 0.0f;

    
}

void DrawPiscadaOlho(float t, int w, int h)
{
    // 't' de 0.0 (aberto) até 1.0 (fechado)
    t = Clamp(t,0.0f,1.0f);
    // Animação suave, pode fazer spring se quiser!
    float easeT = t*t*(3-2*t);

    // Alpha da pálpebra: aberto=0, fechado=1
    unsigned char alpha = (unsigned char)(255.0f * easeT);

    DrawRectangle(0,0,w,h, (Color){0,0,0,alpha});
}

void UpdateCutscenes(void)
{
    if (ended) return;

    if (IsKeyPressed(KEY_SPACE))
    {
        if (!aguardandoFimBgFinal) {
            // Vá direto para a tela final (os valores dependem do seu sistema de estados, adapte!)
            mostrandoBgFinal = true;
            animZoomFinalIniciou = true;
            aguardandoFimBgFinal = true;
            estadoPiscadaAtual = 4; // pula piscadas se quiser
            animZoomFinalStart = GetTime();
            // Faça outras flags de transição se quiser cortar animações intermediárias
            // Você pode remover telas intermediárias aqui também.
        }
        else {
            // Já está esperando, então encerra de verdade
            ended = true;
            aguardandoFimBgFinal = false;
        }
        return;
    }

    //float time = GetTime() - startTime;
    if (aguardandoFimBgFinal) {
        return;
    }
}

// --- HANK Antigo ---
void DrawHankAnim(float animTime, int w, int h)
{
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = personagemHank.width;
    float spriteH = personagemHank.height;
    float escala = (h / 4.0f / spriteH) * 3.5f;
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xFinal = 20.0f;
    float xStart = xFinal - 28.0f;
    float xAtual = xStart + (xFinal - xStart) * easeT;
    float yFinal = h - dstH - 32.0f + 160.0f;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            personagemHank,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xAtual, yFinal, dstW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255,alpha}
        );
    }
}

static void DrawCostasSpritesOverBgHacker(float surgimentoT, int w, int h)
{
    Texture2D sprites[NUM_COSTAS] = {costasJade, costasLevi, costasAlice, costasDante};
    const float sprMargin = 60.0f;
    float totalWidth = 0;
    float desiredH = h * 0.6f;
    float sprW[NUM_COSTAS], sprH[NUM_COSTAS], escala[NUM_COSTAS];

    for (int i = 0; i < NUM_COSTAS; ++i) {
        escala[i] = desiredH / sprites[i].height;
        sprW[i] = sprites[i].width * escala[i];
        sprH[i] = desiredH;
        totalWidth += sprW[i];
        if (i < NUM_COSTAS-1) totalWidth += sprMargin;
    }
    float startX = (w - totalWidth)/2.0f;
    float baseY = h - desiredH;

    // Fade in + subida
    float exibeT = surgimentoT/1.1f;
    if (exibeT > 1.0f) exibeT = 1.0f;
    float smoothT = exibeT*exibeT*(3.0f-2.0f*exibeT);
    float yAppearOffset = (1.0f-smoothT) * (h*0.12f);
    unsigned char alpha = (unsigned char)(255.0f * smoothT);

    // Movimento X SUAVE, sincronizado mas com pequenas fases
    float now = (float)GetTime();
    float floatAmountX = w * 0.020f; // 2% width
    float px = startX;
    float baseFreq = 0.74f;           // ~1 ciclo/seg
    for (int i=0; i<NUM_COSTAS; ++i) {
        // A fase faz cada personagem balançar com atraso SUAVE, não saltos!
        float phase = 0.5f * i; 
        float floX = sinf(now * baseFreq + phase) * floatAmountX;

        float x = px + floX;
        float y = baseY + yAppearOffset;

        DrawTexturePro(
            sprites[i],
            (Rectangle){0,0,sprites[i].width,sprites[i].height},
            (Rectangle){x, y, sprW[i], sprH[i]},
            (Vector2){0,0}, 0.0f, (Color){255,255,255,alpha}
        );
        px += sprW[i] + sprMargin;
    }
}

static void DrawGlitchTransitionBg5ToHacker(float glitchT, int w, int h)
{
    // Deixa o fundo bg5 como base
    DrawTexturePro(
        bg5,
        (Rectangle){0,0,bg5.width,bg5.height},
        (Rectangle){0,0,w,h},
        (Vector2){0,0}, 0.0f, WHITE
    );

    // Sobrepõe GLITCHES e revela partes bgHacker com "corrosão"
    int glitches = 18 + (int)(28 * glitchT);
    for (int i = 0; i < glitches; i++)
    {
        int ry = GetRandomValue(0, h - 5);
        int rh = GetRandomValue(1, 10);
        DrawRectangle(0, ry, w, rh, BLACK);

        if (glitchT > 0.3f) {
            int rx = GetRandomValue(0, w - 60);
            int rw = GetRandomValue(16, 90);
            int ry2 = GetRandomValue(0, h - 20);
            int rh2 = GetRandomValue(8, 30);
            // Ao invés de apagar, "revela" parta do bgHacker
            DrawTexturePro(
                bgHacker,
                (Rectangle){rx * ((float)bgHacker.width / w), ry2 * ((float)bgHacker.height / h), rw * ((float)bgHacker.width / w), rh2 * ((float)bgHacker.height / h)},
                (Rectangle){rx, ry2, rw, rh2},
                (Vector2){0,0}, 0.0f, WHITE
            );
        }
    }

    // FADE OUT para revelar bgHacker por completo
    if (glitchT > 0.70f) {
        unsigned char fadeAlpha = (unsigned char)(255 * (glitchT-0.7f) / 0.3f);
        if (fadeAlpha > 255) fadeAlpha = 255;
        DrawRectangle(0,0,w,h,(Color){0,0,0,fadeAlpha});
    }
}

// Rejeita o Levi: animação de carimbo igual Alice, mas 100px à direita do rejAlice
void DrawAnimacaoCarimboSobreFotosLevi(float elapsed, int w, int h, Rectangle fotosRect, Texture2D carimbo)
{
    // Copiado e adaptado de DrawAnimacaoCarimboSobreFotosAlice

    const float tempoQueda   = 0.36f;
    const float tempoSquash  = 0.14f;
    const float squashScaleY = 0.61f;
    const float squashScaleX = 1.25f;
    float dstW = fotosRect.width / 5.2f;
    float dstH = fotosRect.height / 5.2f;

    // Posição do carimbo Levi: igual Alice + 100 pixels à direita
    float dstX = (fotosRect.x + (fotosRect.width - dstW) * 0.54f) + 510;
    float dstY = (fotosRect.y + (fotosRect.height - dstH) * 0.54f) + 130;
    float carimboW = carimbo.width;
    float carimboH = carimbo.height;
    float squashX = 1.0f;
    float squashY = 1.0f;
    float angulo = 12.0f;
    float fadeIn = 1.0f;
    float yOff = 0;

    if (elapsed < tempoQueda) {
        float t = elapsed / tempoQueda;
        yOff = -fotosRect.height * (1.1f - t);
        angulo = 26.0f - 18.0f * t;
        fadeIn = 0.1f + 0.89f * t;
    }
    else if (elapsed < tempoQueda + tempoSquash) {
        float t = (elapsed - tempoQueda) / tempoSquash;
        squashX = squashScaleX - (squashScaleX - 1) * t;
        squashY = squashScaleY + (1 - squashScaleY) * t;
        yOff = 0.f;
        fadeIn = 1.0f;
        angulo = 8.0f + 7.0f * (1-t);
    }
    else {
        squashX = 1;
        squashY = 1;
        yOff = 0.f;
        fadeIn = 1.0f;
        angulo = 12.0f;
    }

    unsigned char alpha = (unsigned char)(255 * fadeIn);
    float cx = dstX + dstW/2.0f;
    float cy = dstY + dstH/2.0f + yOff;
    Rectangle dst = { cx - (dstW*squashX)/2, cy - (dstH*squashY)/2, dstW*squashX, dstH*squashY };

    DrawTexturePro(
        carimbo,
        (Rectangle){0,0,carimboW,carimboH},
        dst,
        (Vector2){dst.width/2, dst.height/2},
        angulo,
        (Color){255,255,255,alpha}
    );
}

void DrawAnimacaoCarimboSobreFotosAlice(float elapsed, int w, int h, Rectangle fotosRect, Texture2D carimbo)
{
    // Anim: baseada no rejJade, mas desloca +80px para a direita

    const float tempoQueda   = 0.36f;
    const float tempoSquash  = 0.14f;
    const float squashScaleY = 0.61f;
    const float squashScaleX = 1.25f;

    float dstW = fotosRect.width / 5.2f;
    float dstH = fotosRect.height / 5.2f;

    // Base do carimbo Jade:
    // float dstX = (fotosRect.x + (fotosRect.width - dstW) * 0.54f) - 80;
    // float dstY = (fotosRect.y + (fotosRect.height - dstH) * 0.54f) + 150;

    // Para Alice: +80 à direita do Jade:
    float dstX = (fotosRect.x + (fotosRect.width - dstW) * 0.54f) + 160;
    float dstY = (fotosRect.y + (fotosRect.height - dstH) * 0.54f) + 188;

    float carimboW = carimbo.width;
    float carimboH = carimbo.height;
    float squashX = 1.0f;
    float squashY = 1.0f;
    float angulo = 12.0f; // pode escolher outro ângulo se quiser
    float fadeIn = 1.0f;
    float yOff = 0;
    if (elapsed < tempoQueda) {
        float t = elapsed / tempoQueda;
        yOff = -fotosRect.height * (1.1f - t);
        angulo = 26.0f - 18.0f * t; // como Jade
        fadeIn = 0.1f + 0.89f * t;
    }
    else if (elapsed < tempoQueda + tempoSquash) {
        float t = (elapsed - tempoQueda) / tempoSquash;
        squashX = squashScaleX - (squashScaleX - 1) * t;
        squashY = squashScaleY + (1 - squashScaleY) * t;
        yOff = 0.f;
        fadeIn = 1.0f;
        angulo = 8.0f + 7.0f * (1-t);
    }
    else {
        squashX = 1;
        squashY = 1;
        yOff = 0.f;
        fadeIn = 1.0f;
        angulo = 12.0f;
    }
    unsigned char alpha = (unsigned char)(255 * fadeIn);
    float cx = dstX + dstW/2.0f;
    float cy = dstY + dstH/2.0f + yOff;
    Rectangle dst = { cx - (dstW*squashX)/2, cy - (dstH*squashY)/2, dstW*squashX, dstH*squashY };
    DrawTexturePro(
        carimbo,
        (Rectangle){0,0,carimboW,carimboH},
        dst,
        (Vector2){dst.width/2, dst.height/2},
        angulo,
        (Color){255,255,255,alpha}
    );
}

void DrawAnimacaoCarimboSobreFotosJade(float elapsed, int w, int h, Rectangle fotosRect, Texture2D carimbo)
{
    // ANIMAÇÃO DE CARIMBO (JADE)
    const float tempoQueda = 0.36f;
    const float tempoSquash = 0.14f;
    // const float tempoFade = 0.15f; // sem fade out
    const float squashScaleY = 0.61f;
    const float squashScaleX = 1.25f;
    float dstW = fotosRect.width / 6.0f;
    float dstH = fotosRect.height / 6.0f;
    // --- 60px à direita do carimbo anterior --- (+60 no X)
    float dstX = (fotosRect.x + (fotosRect.width - dstW) * 0.54f) - 80;
    float dstY = (fotosRect.y + (fotosRect.height - dstH) * 0.54f) + 150;
    float carimboW = carimbo.width;
    float carimboH = carimbo.height;
    float squashX = 1.0f;
    float squashY = 1.0f;
    float angulo = 6.0f; // <--- ângulo diferente
    float fadeIn = 1.0f;
    float yOff = 0;
    if (elapsed < tempoQueda) {
        float t = elapsed / tempoQueda;
        yOff = -fotosRect.height * (1.1f - t);
        angulo = 26.0f - 18.0f * t; // diferente do Dante
        fadeIn = 0.1f + 0.89f * t;
    }
    else if (elapsed < tempoQueda + tempoSquash) {
        float t = (elapsed - tempoQueda) / tempoSquash;
        squashX = squashScaleX - (squashScaleX - 1) * t;
        squashY = squashScaleY + (1 - squashScaleY) * t;
        yOff = 0.f;
        fadeIn = 1.0f;
        angulo = 8.0f + 7.0f * (1-t);
    }
    else {
        squashX = 1;
        squashY = 1;
        yOff = 0.f;
        fadeIn = 1.0f;
        angulo = 12.0f; // Fixo diferente
    }
    unsigned char alpha = (unsigned char)(255 * fadeIn);
    float cx = dstX + dstW/2.0f;
    float cy = dstY + dstH/2.0f + yOff;
    Rectangle dst = { cx - (dstW*squashX)/2, cy - (dstH*squashY)/2, dstW*squashX, dstH*squashY };
    DrawTexturePro(
        carimbo,
        (Rectangle){0,0,carimboW,carimboH},
        dst,
        (Vector2){dst.width/2, dst.height/2},
        angulo,
        (Color){255,255,255,alpha}
    );
}

void DrawLeviAnim(float animTime, int w, int h)
{
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = personagemLevi.width;
    float spriteH = personagemLevi.height;
    float escala = 0.45f;
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xFinal = (w - dstW) / 2.0f;    // CENTRALIZADO
    //float xStart = xFinal;
    float yFinal = h - dstH - 32.0f;
    float yStart = yFinal + 150.0f;        // Aparece de baixo para cima
    float yAtual = yStart + (yFinal - yStart) * easeT;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            personagemLevi,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yAtual, dstW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255, alpha}
        );
    }
}



void DrawV1LeviAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v1_levi.width;
    float spriteH = v1_levi.height;
    // Centralize o "personagemLevi" no centro (como já faz)
    float escalaLevi = 1.0f; // Mesmo do personagemLevi central
    float dstLW = personagemLevi.width * escalaLevi;
    float dstLH = personagemLevi.height * escalaLevi;
    float escala = 0.32f; // Clone menorzinho porém visível
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xLevi = (w - dstLW) / 2.0f;
    float yLevi = h - dstLH - 32.0f;
    // POSICIONE ao lado esquerdo do Levi central, mas pertinho dele:
    float xFinal = xLevi - dstW * 0.75f;
    float yFinal = yLevi + dstLH - dstH - 520;
    // -- Anima igual
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 60.0f;
        float yStart = yFinal - 70.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v1_levi,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 0.0f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v1_levi,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

void DrawV2LeviAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v2_levi.width;
    float spriteH = v2_levi.height;
    float escalaLevi = 1.0f;
    float dstLW = personagemLevi.width * escalaLevi;
    float dstLH = personagemLevi.height * escalaLevi;
    float escala = 0.4f; // Um pouco menor
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xLevi = (w - dstLW) / 2.0f;
    float yLevi = h - dstLH - 32.0f;
    // GRUDA mais à esquerda do V1 Levi:
    float xFinal = xLevi - dstW * 0.75f + 600;
    float yFinal = yLevi + dstLH - dstH - 680;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 40.0f;
        float yStart = yFinal - 80.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v2_levi,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 1.22f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v2_levi,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

void DrawV3LeviAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v3_levi.width;
    float spriteH = v3_levi.height;
    float escalaLevi = 1.0f;
    float dstLW = personagemLevi.width * escalaLevi;
    float dstLH = personagemLevi.height * escalaLevi;
    float escala = 0.3f;
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xLevi = (w - dstLW) / 2.0f;
    float yLevi = h - dstLH - 32.0f;
    // MAIS À ESQUERDA
    float xFinal = xLevi - dstW * 0.75f +1300;
    float yFinal = yLevi + dstLH - dstH - 530;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 30.0f;
        float yStart = yFinal - 60.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v3_levi,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 2.17f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v3_levi,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

void DrawNomeLeviAnim(float animTime, int w, int h)
{
    const float animDur = 1.00f;
    float t = animTime / animDur;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float easeT = 1.0f - powf(1.0f-t, 2.2f);
    float texW = nomeLevi.width;
    float texH = nomeLevi.height;
    float escala = ((float)w / 4.5f) / texW;
    if (escala * texH > h / 9.5f) escala = (h/1.5f)/texH;
    float dstW = texW * escala;
    float dstH = texH * escala;
    float xDest = (w - dstW) / 2.0f - 600; // CENTRALIZADO
    float yDest = h - dstH + 200.0f; // Ajuste se quiser +perto/+longe de Levi
    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (easeT > 0.0f)
    {
        DrawTexturePro(
            nomeLevi,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255,alpha}
        );
    }
}

// ---- Animacao Alice: IGUAL Hank, mas vindo da direita ----
void DrawAliceAnim(float animTime, int w, int h)
{
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = personagemAlice.width;
    float spriteH = personagemAlice.height;
    float escala = (h / 4.0f / spriteH) * 3.6f;
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xFinal = w - dstW - 20.0f; // <- final à direita, afastado 20px
    float xStart = xFinal + 28.0f;   // <- começa mais pra fora da tela ainda
    float xAtual = xStart + (xFinal - xStart) * easeT;
    float yFinal = h - dstH - 32.0f + 260.0f;   // = igual Hank
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            personagemAlice,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xAtual, yFinal, dstW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255,alpha}
        );
    }
}

// --- v1_alice animada próxima de Alice, menor e à esquerda dela, e depois flutuando ---
void DrawV1AliceAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return; // <-- Garante que não desenha com animTime negativo!
    float spriteW = v1_alice.width;
    float spriteH = v1_alice.height;
    float escalaAlice = (h / 4.0f / personagemAlice.height) * 3.5f;
    float dstAW = personagemAlice.width * escalaAlice;
    float dstAH = personagemAlice.height * escalaAlice;
    float escala = escalaAlice * (1.0f / 3.5f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xAlice = w - dstAW - 20.0f;
    float yAlice = h - dstAH - 32.0f + 160.0f;
    float xFinal = xAlice - dstW * 0.78f - 120;
    float yFinal = yAlice - dstH * 0.33f;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 80.0f;
        float yStart = yFinal - 100.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v1_alice,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 0.0f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v1_alice,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

void DrawV2AliceAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return; // <-- Aqui também!
    float spriteW = v2_alice.width;
    float spriteH = v2_alice.height;
    float escalaAlice = (h / 4.0f / personagemAlice.height) * 3.5f;
    float dstAW = personagemAlice.width * escalaAlice;
    float dstAH = personagemAlice.height * escalaAlice;
    float escala = escalaAlice * (1.0f / 3.0f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xAlice = w - dstAW - 20.0f;
    float yAlice = h - dstAH - 32.0f + 160.0f;
    float xV1Final = xAlice - (v1_alice.width * escalaAlice * (1.0f / 3.5f)) * 0.78f - 120;
    float yV1Final = yAlice - (v1_alice.height * escalaAlice * (1.0f / 3.5f)) * 0.33f;
    float offsetX = -(dstW * 0.95f + 70);
    float offsetY = -40.0f - 35;
    float xFinal = xV1Final + offsetX;
    float yFinal = yV1Final + offsetY;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 70.0f;
        float yStart = yFinal - 80.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v2_alice,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 1.22f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v2_alice,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

void DrawV3AliceAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    float spriteW3 = v3_alice.width;
    float spriteH3 = v3_alice.height;
    float escalaAlice = (h / 4.0f / personagemAlice.height) * 3.5f;
    float escala2 = escalaAlice * (1.0f / 3.0f);
    float dstW2 = v2_alice.width * escala2;
    //float dstH2 = v2_alice.height * escala2;
    float escala3 = escalaAlice * (1.0f / 2.2f);
    float dstW3 = spriteW3 * escala3;
    float dstH3 = spriteH3 * escala3;
    float dstAW = personagemAlice.width * escalaAlice;
    float dstAH = personagemAlice.height * escalaAlice;
    float xAlice = w - dstAW - 20.0f;
    float yAlice = h - dstAH - 32.0f + 160.0f;
    float escala1 = escalaAlice * (1.0f / 3.5f);
    float xV1Final = xAlice - (v1_alice.width * escala1) * 0.78f - 120;
    float yV1Final = yAlice - (v1_alice.height * escala1) * 0.33f;
    float offsetX_v2 = -(dstW2 * 0.95f + 70);
    float offsetY_v2 = -40.0f - 35;
    float xV2Final = xV1Final + offsetX_v2;
    float yV2Final = yV1Final + offsetY_v2;
    float offsetX_v3 = -dstW3 * 0.99f - 90;
    float offsetY_v3 = +78.0f;
    float xFinal = xV2Final + offsetX_v3;
    float yFinal = yV2Final + offsetY_v3;

    // --- Ajuste principal: não deixa alpha passar de 255, nem ser desenhado com 0 ---
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        if (t > 1.0f) t = 1.0f;
        else if (t < 0.0f) t = 0.0f;
        float easeT = t * t * (3.0f - 2.0f * t); // Smoothstep
        if (easeT > 1.0f) easeT = 1.0f;
        else if (easeT < 0.0f) easeT = 0.0f;
        unsigned char alpha = (unsigned char)(255.0f * easeT + 0.5f); // Round

        if (alpha > 0)
        {
            float xStart = xFinal - 60.0f;
            float yStart = yFinal + 80.0f;
            float xAtual = xStart + (xFinal - xStart) * easeT;
            float yAtual = yStart + (yFinal - yStart) * easeT;
            DrawTexturePro(
                v3_alice,
                (Rectangle){0, 0, spriteW3, spriteH3},
                (Rectangle){xAtual, yAtual, dstW3, dstH3},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 2.17f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v3_alice,
            (Rectangle){0, 0, spriteW3, spriteH3},
            (Rectangle){xFinal, yFinal + offset, dstW3, dstH3},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}
void DrawDanteAnim(float animTime, int w, int h)
{
    // Dante animado vindo da esquerda para o canto inferior esquerdo
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = personagemDante.width;
    float spriteH = personagemDante.height;
    float escala = (h / 1.4f / spriteH);  // ajuste de escala geral
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;

    // Alinhado no canto inferior esquerdo
    float xFinal = 50.0f;                   // 20 pixels da esquerda
    float xStart = xFinal - 28.0f;          // começa um pouco mais fora da tela, à esquerda
    float xAtual = xStart + (xFinal - xStart) * easeT;

    float yFinal = h - dstH - 20.0f;        // 20 pixels do rodapé da tela

    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            personagemDante,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xAtual, yFinal, dstW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255,alpha}
        );
    }
}

// --- Agora V1 tem a posição que era da V3 anterior ---
void DrawV1DanteAnim(float animTime, int w, int h) {
    const float animDuration = 0.12f;
    float spriteW3 = v1_dante.width;
    float spriteH3 = v1_dante.height;
    float escalaDante = (h / 4.0f / personagemDante.height) * 3.5f;
    float escala2 = escalaDante * (1.0f / 5.5f);
    float dstW2 = v2_dante.width * escala2;
    float escala3 = escalaDante * (1.0f / 5.5f);
    float dstW3 = spriteW3 * escala3;
    float dstH3 = spriteH3 * escala3;
    float dstAW = personagemDante.width * escalaDante;
    float dstAH = personagemDante.height * escalaDante;
    float xDante = w - dstAW + 80.0f;
    float yDante = h - dstAH - 32.0f + 160.0f;
    float escala1 = escalaDante * (1.0f / 5.5f);
    float xV1Final = xDante - (v1_dante.width * escala1) * 0.78f - 120;
    float yV1Final = yDante - (v1_dante.height * escala1) * 0.33f;
    float offsetX_v2 = -(dstW2 * 0.95f + 70);
    float offsetY_v2 = -40.0f - 35;
    float xV2Final = xV1Final + offsetX_v2;
    float yV2Final = yV1Final + offsetY_v2;
    float offsetX_v3 = -dstW3 * 0.99f - 90;
    float offsetY_v3 = +78.0f;
    float xFinal = xV2Final + offsetX_v3;
    float yFinal = yV2Final + offsetY_v3;
    if (animTime < animDuration) {
        float t = animTime / animDuration;
        if (t > 1.0f) t = 1.0f;
        else if (t < 0.0f) t = 0.0f;
        float easeT = t * t * (3.0f - 2.0f * t);
        if (easeT > 1.0f) easeT = 1.0f;
        else if (easeT < 0.0f) easeT = 0.0f;
        unsigned char alpha = (unsigned char)(255.0f * easeT + 0.5f);
        if (alpha > 0) {
            float xStart = xFinal - 60.0f;
            float yStart = yFinal + 80.0f;
            float xAtual = xStart + (xFinal - xStart) * easeT;
            float yAtual = yStart + (yFinal - yStart) * easeT;
            DrawTexturePro(
                v1_dante,
                (Rectangle){0, 0, spriteW3, spriteH3},
                (Rectangle){xAtual, yAtual, dstW3, dstH3},
                (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
            );
        }
    } else {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 2.17f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v1_dante,
            (Rectangle){0, 0, spriteW3, spriteH3},
            (Rectangle){xFinal, yFinal + offset, dstW3, dstH3},
            (Vector2){0, 0}, 0.0f, WHITE
        );
    }
}

void DrawV2DanteAnim(float animTime, int w, int h) {
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v2_dante.width;
    float spriteH = v2_dante.height;
    float escalaDante = (h / 4.0f / personagemDante.height) * 3.5f;
    float dstAW = personagemDante.width * escalaDante;
    float dstAH = personagemDante.height * escalaDante;
    float escala = escalaDante * (1.0f / 5.5f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xDante = w - dstAW + 120.0f;
    float yDante = h - dstAH - 32.0f + 160.0f;
    float xV1Final = xDante - (v1_dante.width * escalaDante * (1.0f / 5.5f)) * 0.78f - 120;
    float yV1Final = yDante - (v1_dante.height * escalaDante * (1.0f / 5.5f)) * 0.33f;
    float offsetX = -(dstW * 0.95f + 70);
    float offsetY = -40.0f - 35;
    float xFinal = xV1Final + offsetX;
    float yFinal = yV1Final + offsetY;
    if (animTime < animDuration) {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 70.0f;
        float yStart = yFinal - 80.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0) {
            DrawTexturePro(
                v2_dante,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
            );
        }
    } else {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 1.22f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v2_dante,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0}, 0.0f, WHITE
        );
    }
}

// --- Agora V3 tem a posição que era da V1 anterior ---
void DrawV3DanteAnim(float animTime, int w, int h) {
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v3_dante.width;
    float spriteH = v3_dante.height;
    float escalaDante = (h / 4.0f / personagemDante.height) * 3.5f;
    float dstAW = personagemDante.width * escalaDante;
    float dstAH = personagemDante.height * escalaDante;
    float escala = escalaDante * (1.0f / 5.5f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xDante = w - dstAW + 280.0f;
    float yDante = h - dstAH - 32.0f + 420.0f;
    float xFinal = xDante - dstW * 0.78f - 120;
    float yFinal = yDante - dstH * 0.33f;
    if (animTime < animDuration) {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - 80.0f;
        float yStart = yFinal - 100.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0) {
            DrawTexturePro(
                v3_dante,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
            );
        }
    } else {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 0.0f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v3_dante,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0}, 0.0f, WHITE
        );
    }
}
void DrawNomeDanteAnim(float animTime, int w, int h)
{
    const float animDur = 1.00f;
    float t = animTime / animDur;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float easeT = 1.0f - powf(1.0f-t, 2.2f);
    float texW = nomeDante.width;
    float texH = nomeDante.height;
    float escala = ((float)w / 5.0f) / texW;
    if (escala * texH > h / 9.0f) escala = (h/1.5f)/texH;
    float dstW = texW * escala;
    float dstH = texH * escala;

    // Ajuste para canto superior direito
    float margemX = - 260.0f; // Distância da direita
    float margemY = - 150.0f; // Distância do topo
    float xDest = w - dstW - margemX;
    float yDest = margemY;

    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (easeT > 0.0f)
    {
        DrawTexturePro(
            nomeDante,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
        );
    }
}

void DrawNomeJadeAnim(float animTime, int w, int h)
{
    const float animDur = 1.00f;
    float t = animTime / animDur;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float easeT = 1.0f - powf(1.0f - t, 2.2f);
    float texW = nomeJade.width;
    float texH = nomeJade.height;
    float escala = ((float)w / 5.0f) / texW;
    if (escala * texH > h / 9.0f) escala = (h/1.5f)/texH;
    float dstW = texW * escala;
    float dstH = texH * escala;
    
    // NOVO: margem esquerda (xDest = 0) e centralização vertical
    float xDest = -260.0f;
    float yDest = (h - dstH) / 2.0f;
    
    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (easeT > 0.0f)
    {
        DrawTexturePro(
            nomeJade,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255,alpha}
        );
    }
}
void DrawV1JadeAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v1_jade.width;
    float spriteH = v1_jade.height;

    float escalaJade = (h / 4.0f / personagemJade.height) * 3.5f;
    float dstAW = personagemJade.width * escalaJade;
    float dstAH = personagemJade.height * escalaJade;
    float escala = escalaJade * (1.0f / 5.0f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xJade = 620.0f; // mesmo xFinal do pessoal na base canto
    float yJade = h - dstAH + 320.0f;
    float xFinal = xJade + dstAW * 0.78f + 120;
    float yFinal = yJade - dstH * 0.33f;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal + 80.0f;
        float yStart = yFinal - 100.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v1_jade,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 0.0f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v1_jade,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0}, 0.0f, WHITE
        );
    }
}

void DrawV2JadeAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;
    float spriteW = v2_jade.width;
    float spriteH = v2_jade.height;

    float escalaJade = (h / 4.0f / personagemJade.height) * 3.5f;
    float dstAW = personagemJade.width * escalaJade;
    float dstAH = personagemJade.height * escalaJade;
    float escala = escalaJade * (1.0f / 4.8f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xJade = - 560.0f;
    float yJade = h - dstAH + 50.0f;
    float xV1Final = xJade + dstAW * 0.78f + 120;
    float yV1Final = yJade - dstH * 0.33f;
    float offsetX = dstW * 0.95f + 70;
    float offsetY = 20.0f + 40.0f;
    float xFinal = xV1Final + offsetX;
    float yFinal = yV1Final + offsetY;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal + 70.0f;
        float yStart = yFinal + 70.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v2_jade,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 1.22f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v2_jade,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0}, 0.0f, WHITE
        );
    }
}

void DrawV3JadeAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    if (animTime < 0.0f) return;

    float spriteW = v3_jade.width;
    float spriteH = v3_jade.height;
    float escalaJade = (h / 4.0f / personagemJade.height) * 3.5f;
    float escala = escalaJade * (1.0f / 5.5f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;

    float margemX = 280.0f;        // Distância da esquerda
    float margemInferior = -74.0f; // Distância do rodapé (negativo = mais pra cima)

    float xFinal = margemX;
    float yFinal = h - dstH - margemInferior;

    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal - dstW - 80.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yFinal;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v3_jade,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0}, 0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 2.17f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;

        DrawTexturePro(
            v3_jade,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0}, 0.0f, WHITE
        );
    }
}

// --- v1_hank animada próxima de Hank, menor e depois dele, e depois flutuando ---
void DrawV1HankAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    float spriteW = v1_hank.width;
    float spriteH = v1_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 2.8f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala = escalaHank * (1.0f / 3.5f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xHank = 20.0f;
    float yHank = h - dstHH + 10.0f;
    float xFinal = xHank + dstHW * 0.78f +120;
    float yFinal = yHank - dstH * 0.33f;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal + 80.0f;
        float yStart = yFinal - 100.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v1_hank,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 0.0f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v1_hank,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

// ----------- v2_hank animada (com flutuação depois) -------------
void DrawV2HankAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    float spriteW = v2_hank.width;
    float spriteH = v2_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 2.8f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala = escalaHank * (1.0f / 1.6f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xHank = 20.0f;
    float yHank = h - dstHH + 60.0f;
    float xV1Final = xHank + dstHW * 0.78f + 120;
    float yV1Final = yHank - dstH * 0.33f;
    float offsetX = dstW * 0.95f + 70;
    float offsetY = 20.0f +40;
    float xFinal = xV1Final + offsetX;
    float yFinal = yV1Final + offsetY;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal + 70.0f;
        float yStart = yFinal + 70.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v2_hank,
                (Rectangle){0, 0, spriteW, spriteH},
                (Rectangle){xAtual, yAtual, dstW, dstH},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 1.22f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v2_hank,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xFinal, yFinal + offset, dstW, dstH},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

// ---- v3_hank animada: à direita de v2_hank, com flutuação -----
void DrawV3HankAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    float spriteW2 = v2_hank.width;
    float spriteH2 = v2_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 2.8f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala2 = escalaHank * (1.0f / 1.4f);
    float dstW2 = spriteW2 * escala2;
    float dstH2 = spriteH2 * escala2;
    float xHank = 20.0f;
    float yHank = h - dstHH - 80;
    float xV1Final = xHank + dstHW * 0.78f + 120;
    float yV1Final = yHank - dstH2 * 0.33f;
    float offsetX2 = dstW2 * 0.95f + 350;
    float offsetY2 = 20.0f + 20;
    float xV2Final = xV1Final + offsetX2;
    float yV2Final = yV1Final + offsetY2;
    float spriteW3 = v3_hank.width;
    float spriteH3 = v3_hank.height;
    float escala3 = escalaHank * (1.0f / 1.2f);
    float dstW3 = spriteW3 * escala3;
    float dstH3 = spriteH3 * escala3;
    float xFinal = xV2Final + 70.0f;
    float yFinal = yV2Final - 50.0f;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal + 70.0f;
        float yStart = yFinal - 60.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v3_hank,
                (Rectangle){0, 0, spriteW3, spriteH3},
                (Rectangle){xAtual, yAtual, dstW3, dstH3},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 2.17f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v3_hank,
            (Rectangle){0, 0, spriteW3, spriteH3},
            (Rectangle){xFinal, yFinal + offset, dstW3, dstH3},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

// --- v4_hank animada à direita/abaixo de v3_hank, e depois flutuação ---
void DrawV4HankAnim(float animTime, int w, int h)
{
    const float animDuration = 0.12f;
    float spriteW2 = v2_hank.width;
    float spriteH2 = v2_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 2.8f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala2 = escalaHank * (1.0f / 1.4f);
    float dstW2 = spriteW2 * escala2;
    float dstH2 = spriteH2 * escala2;
    float xHank = 20.0f;
    float yHank = h - dstHH  + 100.0f;
    float xV1Final = xHank + dstHW * 0.78f + 120;
    float yV1Final = yHank - dstH2 * 0.33f;
    float offsetX2 = dstW2 * 0.95f + 350;
    float offsetY2 = 20.0f + 20;
    float xV2Final = xV1Final + offsetX2;
    float yV2Final = yV1Final + offsetY2;
    float xV3Final = xV2Final + 70.0f;
    float yV3Final = yV2Final - 50.0f;
    float spriteW4 = v4_hank.width;
    float spriteH4 = v4_hank.height;
    float escala4 = escalaHank * (1.0f / 3.5f);
    float dstW4 = spriteW4 * escala4;
    float dstH4 = spriteH4 * escala4;
    float xFinal = xV3Final + 300.0f;
    float yFinal = yV3Final + 170.0f;
    if (animTime < animDuration)
    {
        float t = animTime / animDuration;
        float easeT = t * t * (3.0f - 2.0f * t);
        float xStart = xFinal + 50.0f;
        float yStart = yFinal + 100.0f;
        float xAtual = xStart + (xFinal - xStart) * easeT;
        float yAtual = yStart + (yFinal - yStart) * easeT;
        unsigned char alpha = (unsigned char)(255 * easeT);
        if (alpha > 0)
        {
            DrawTexturePro(
                v4_hank,
                (Rectangle){0, 0, spriteW4, spriteH4},
                (Rectangle){xAtual, yAtual, dstW4, dstH4},
                (Vector2){0, 0},
                0.0f, (Color){255,255,255,alpha}
            );
        }
    }
    else
    {
        float floatingFreq = 2.0f;
        float floatingAmp = 8.0f;
        float phase = 3.10f;
        float offset = sinf(GetTime() * floatingFreq + phase) * floatingAmp;
        DrawTexturePro(
            v4_hank,
            (Rectangle){0, 0, spriteW4, spriteH4},
            (Rectangle){xFinal, yFinal + offset, dstW4, dstH4},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
}

// ----------- Animação do nome Hank depois de v4_hank finalizar -----------
void DrawNomeHankAnim(float animTime, int w, int h)
{
    const float animDur = 1.00f;
    float t = animTime / animDur;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float easeT = 1.0f - powf(1.0f-t, 2.2f);
    float texW = nomeHank.width;
    float texH = nomeHank.height;
    float escala = ((float)w / 4.5f) / texW;
    if (escala * texH > h / 9.0f) escala = (h/1.5f)/texH;
    float dstW = texW * escala;
    float dstH = texH * escala;
    float margemX = 220.0f;
    float margemY = 160.0f;
    float xDest = w - dstW + margemX;
    float yDest = h - dstH + margemY;
    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (easeT > 0.0f)
    {
        DrawTexturePro(
            nomeHank,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0},
            0.0f,
            (Color){255,255,255,alpha}
        );
    }
}
static void DrawZoomOutBgFinal(float t, int w, int h)
{
    // t: 0.0 (zoom máximo, só o centro) -- até -- 1.0 (imagem normal)
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    // SmoothStep para suavizar
    float easeT = t * t * (3.0f - 2.0f * t);

    float srcW = bgFinal.width;
    float srcH = bgFinal.height;
    //float dstW = (float)w;
    //float dstH = (float)h;
    // Zoom máximo: escolha quanto de zoom dá no começo (ex: 2.0x, pode ser mais/melhor)
    float maxZoom = 2.65f;

    // Zoom "atual" (começa em maxZoom, termina em 1.0)
    float zoom = maxZoom - (maxZoom - 1.0f) * easeT;

    // --- Centraliza o recorte ---
    float cropW = srcW / zoom;
    float cropH = srcH / zoom;
    float cropX = (srcW - cropW) / 2.0f;
    float cropY = (srcH - cropH) / 2.0f;

    // (Opcional: fade-in!)
    unsigned char alpha = 255;
    // Exemplo de fade'n com t<0.10:
    if (t < 0.08f) alpha = (unsigned char) (255*(t/0.08f));
    else alpha = 255;

    DrawTexturePro(
        bgFinal,
        (Rectangle){cropX, cropY, cropW, cropH},
        (Rectangle){0, 0, w, h},
        (Vector2){0, 0},
        0.0f,
        (Color){255,255,255, alpha}
    );
}
void DrawNomeAliceAnim(float animTime, int w, int h)
{
    const float animDur = 1.00f;
    float t = animTime / animDur;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    float easeT = 1.0f - powf(1.0f-t, 2.2f);
    float texW = nomeAlice.width;
    float texH = nomeAlice.height;
    float escala = ((float)w / 4.5f) / texW;
    if (escala * texH > h / 9.0f) escala = (h/1.5f)/texH;
    float dstW = texW * escala;
    float dstH = texH * escala;
    float margemX = -200.0f;
    float margemY = 50.0f;
    float xDest = margemX;                         // LADO ESQUERDO
    float yDest = h - dstH + margemY;
    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (easeT > 0.0f)
    {
        DrawTexturePro(
            nomeAlice,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0},
            0.0f,
            (Color){255,255,255,alpha}
        );
    }
}

// --- (Demais funções extras, alterações apenas na DrawCutscenes) ---

void DrawEmpresa4DetetiveAnim(float animTime, int w, int h)
{
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = detetiveNaEmpresa4.width;
    float spriteH = detetiveNaEmpresa4.height;
    float escala = (h/4.0f/spriteH) * 3.5f;
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xFinal = -15.0f;
    float xStart = xFinal - 15.0f;
    float xAtual = xStart + (xFinal - xStart) * easeT;
    float yFinal = h * 0.72f - dstH + 280.0f;
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            detetiveNaEmpresa4,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xAtual, yFinal, dstW, dstH},
            (Vector2){0, 0},
            0.0f,
            (Color){255, 255, 255, alpha}
        );
    }
}

void DrawEmpresa4TintaAnimESelo(float showTime, int w, int h)
{
    const float tintaAnimDur = 1.1f;
    float tintaT = showTime / tintaAnimDur;
    if (tintaT > 1.0f) tintaT = 1.0f;
    if (tintaT < 0.0f) tintaT = 0.0f;
    float easeT = 1.0f - powf(1.0f-tintaT, 2.2f);
    float texW = minhaTinta.width;
    float texH = minhaTinta.height;
    float escala = (h/8.0f/texH)*11.0f;
    float dstW = texW * escala;
    float dstH = texH * escala;
    float xDest = w - dstW + 400.0f;
    float yDest = -410.0f;
    float revealedW = dstW * easeT;
    float revealedPixSrc = texW * easeT;
    unsigned char alpha = (unsigned char)(200 + 55 * easeT);
    if (tintaT > 0.0f)
    {
        DrawTexturePro(
            minhaTinta,
            (Rectangle){0, 0, revealedPixSrc, texH},
            (Rectangle){xDest, yDest, revealedW, dstH},
            (Vector2){0, 0},
            0.0f,
            (Color){255, 255, 255, alpha}
        );
    }
    if (tintaT >= 0.999f) {
        float selarAnimTime = showTime - tintaAnimDur;
        const float selarAnimDur = 0.42f;
        float carimboT = selarAnimTime / selarAnimDur;
        if (carimboT > 1.0f) carimboT = 1.0f;
        if (carimboT < 0.0f) carimboT = 0.0f;
        float easeA = carimboT * carimboT * (3.0f - 2.0f * carimboT);
        float spring = 1.05f - 0.10f * sinf(easeA * PI);
        float seloW = meuSelo.width;
        float seloH = meuSelo.height;
        float escalaSeloBase = (dstW < dstH ? dstW : dstH) * 0.28f / ((seloW > seloH) ? seloW : seloH);
        float escalaSelo = escalaSeloBase * (0.70f + 0.30f * easeA) * spring;
        if (escalaSelo > escalaSeloBase) escalaSelo = escalaSeloBase;
        float seloDstW = seloW * escalaSelo;
        float seloDstH = seloH * escalaSelo;
        float seloX = xDest + dstW/2.0f - seloDstW/2.0f;
        float seloY = yDest + dstH/2.0f - seloDstH/2.0f;
        unsigned char alphaSelo = (unsigned char)(255 * easeA);
        DrawTexturePro(
            meuSelo,
            (Rectangle){0,0,seloW,seloH},
            (Rectangle){seloX, seloY, seloDstW, seloDstH},
            (Vector2){0,0},
            0.0f,
            (Color){255,255,255,alphaSelo}
        );
    }
}

void DrawAnimacaoPinceladaBg1(float elapsed, int w, int h)
{
    const float duracao = 1.10f;
    const int pinceladas = 9;
    float t = elapsed / duracao;
    if (t > 1.0f) t = 1.0f;
    float srcW = empresa4.width, srcH = empresa4.height;
    float scaleX = srcW / (float)w;
    float scaleY = srcH / (float)h;
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    float cropW = w * scale;
    float cropH = h * scale;
    float cropX = 0;
    float cropY = (srcH - cropH) * 0.5f;
    DrawTexturePro(
        empresa4,
        (Rectangle){cropX, cropY, cropW, cropH},
        (Rectangle){0, 0, w, h},
        (Vector2){0, 0},
        0.0f, WHITE
    );
    const float detetiveDur = 1.2f;
    const float tintaDelay = detetiveDur;
    float empresa4ShowTime = elapsed;
    float detetiveAnimTime = empresa4ShowTime;
    if (detetiveAnimTime < 0.0f) detetiveAnimTime = 0.0f;
    if (detetiveAnimTime > detetiveDur) detetiveAnimTime = detetiveDur;
    DrawEmpresa4DetetiveAnim(detetiveAnimTime, w, h);
    if (empresa4ShowTime >= tintaDelay)
        DrawEmpresa4TintaAnimESelo(empresa4ShowTime - tintaDelay, w, h);
    int barraAltura = (h + pinceladas - 1) / pinceladas;
    float faixa_delay = 0.015f;
    float faixa_duracao = 0.75f;
    int overlap = barraAltura / 7;
    for (int i = 0; i < pinceladas; i++) {
        float inicio = i * faixa_delay;
        float faixaT = (elapsed - inicio) / faixa_duracao;
        if (faixaT < 0) faixaT = 0;
        if (faixaT > 1) faixaT = 1;
        faixaT = faixaT*faixaT*(3-2*faixaT);
        int revelado = (int)(w * faixaT);
        int x = (i % 2 == 0) ? 0 : w - revelado;
        int y = i * barraAltura;
        int barraAlturaReal = barraAltura + ((i < pinceladas-1) ? overlap : 0);
        DrawTexturePro(
            bg1,
            (Rectangle){
                (float)x * ((float)bg1.width/(float)w),
                (float)y * ((float)bg1.height/(float)h),
                revelado * ((float)bg1.width/(float)w),
                barraAlturaReal * ((float)bg1.height/(float)h)},
            (Rectangle){x, y, revelado, barraAlturaReal},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
    if (elapsed >= (faixa_delay * (pinceladas-1)) + faixa_duracao) {
        pinceladaTerminou = true;
        if(momentoBG1Inteiro == 0.0f)
            momentoBG1Inteiro = GetTime();
    }
}

static void DrawAnimacaoBarrasBg5(float elapsed, int w, int h)
{
    const int bars = 96;
    float t = elapsed / 1.15f;
    if (t > 1.0f) t = 1.0f;
    int barWidth = (w + bars - 1) / bars;

    // Desenha bg4 como base
    DrawTexturePro(
        bg4,
        (Rectangle){0,0,bg4.width,bg4.height},
        (Rectangle){0,0,w,h},
        (Vector2){0,0}, 0.0f, WHITE
    );
    for (int i = 0; i < bars; i++)
    {
        float barraInicio = (float)i / bars * 0.55f;
        float randomSeed = (float)((i * 193) % 211) / 211.0f;
        barraInicio += randomSeed * 0.12f;
        float barraT = (t - barraInicio) / 0.3f;
        if (barraT < 0) barraT = 0;
        if (barraT > 1) barraT = 1;
        barraT = barraT*barraT*(3-2*barraT);
        int yReveal = (int)(h * barraT);

        int x = w - (i+1)*barWidth; // De trás pra frente

        if (yReveal > 0)
        {
            DrawTexturePro(
                bg5,
                (Rectangle){
                    (float)x * ((float)bg5.width/w),
                    0,
                    barWidth * ((float)bg5.width/w),
                    yReveal * ((float)bg5.height/h)},
                (Rectangle){x,
                            0,
                            barWidth,
                            yReveal},
                (Vector2){0,0}, 0.0f, WHITE
            );
        }
    }
}

static void DrawAnimacaoBarrasBg4(float elapsed, int w, int h)
{
    const int bars = 96;
    float t = elapsed / 1.15f;
    if (t > 1.0f) t = 1.0f;
    int barWidth = (w + bars - 1) / bars;
    // Desenha bg3 como base e revela o bg4 por barras aumentando da esquerda p/direita
    DrawTexturePro(
        bg3,
        (Rectangle){0,0,bg3.width,bg3.height},
        (Rectangle){0,0,w,h},
        (Vector2){0,0}, 0.0f, WHITE
    );
    for (int i = 0; i < bars; i++)
    {
        float barraInicio = (float)i / bars * 0.55f;
        float randomSeed = (float)((i * 193) % 211) / 211.0f;
        barraInicio += randomSeed * 0.12f;
        float barraT = (t - barraInicio) / 0.3f;
        if (barraT < 0) barraT = 0;
        if (barraT > 1) barraT = 1;
        barraT = barraT*barraT*(3-2*barraT);
        int yReveal = (int)(h * barraT);
        int x = i * barWidth;
        if (yReveal > 0)
        {
            DrawTexturePro(
                bg4,
                (Rectangle){
                    (float)x * ((float)bg4.width/w),
                    0,
                    barWidth * ((float)bg4.width/w),
                    yReveal * ((float)bg4.height/h)},
                (Rectangle){x, 0, barWidth, yReveal},
                (Vector2){0,0}, 0.0f, WHITE
            );
        }
    }
}

// --- MOD: Transição para BG2, barras verticais tipo empresa1
static void DrawAnimacaoBarrasBg2(float elapsed, int w, int h)
{
    const int bars = 96;
    float t = elapsed / 1.15f;
    if (t > 1.0f) t = 1.0f;
    int barWidth = (w + bars - 1) / bars;

    // Desenha bg1 como base e revela o bg2 pelas barras.
    DrawTexturePro(
        bg1,
        (Rectangle){0,0,bg1.width,bg1.height},
        (Rectangle){0,0,w,h},
        (Vector2){0,0}, 0.0f, WHITE
    );
    for (int i = 0; i < bars; i++)
    {
        float barraInicio = (float)i / bars * 0.55f;
        float randomSeed = (float)((i * 193) % 211) / 211.0f;
        barraInicio += randomSeed * 0.12f;
        float barraT = (t - barraInicio) / 0.3f;
        if (barraT < 0) barraT = 0;
        if (barraT > 1) barraT = 1;
        barraT = barraT * barraT * (3.0f - 2.0f * barraT);
        int yReveal = (int)(h * barraT);

        if (yReveal > 0)
        {
            // bg2 strip
            DrawTexturePro(
                bg2,
                (Rectangle){
                    (float)(i*barWidth) * ((float)bg2.width/w),
                    0,
                    barWidth * ((float)bg2.width/w),
                    yReveal * ((float)bg2.height/h)},
                (Rectangle){i*barWidth, 0, barWidth, yReveal},
                (Vector2){0,0}, 0.0f, WHITE
            );
        }
    }
}
static void DrawAnimacaoBarrasBg3(float elapsed, int w, int h)
{
    const int bars = 64;
    float t = elapsed / 1.15f;
    if (t > 1.0f) t = 1.0f;
    int barHeight = (h + bars - 1) / bars;
    // Desenha bg2 como base e revela o bg3 por barras de cima para baixo
    DrawTexturePro(
        bg2,
        (Rectangle){0,0,bg2.width,bg2.height},
        (Rectangle){0,0,w,h},
        (Vector2){0,0}, 0.0f, WHITE
    );
    for (int i = 0; i < bars; i++) {
        float barraInicio = (float)i / bars * 0.55f;
        float randomSeed = (float)((i * 193) % 211) / 211.0f;
        barraInicio += randomSeed * 0.12f;
        float barraT = (t - barraInicio) / 0.3f;
        if (barraT < 0) barraT = 0;
        if (barraT > 1) barraT = 1;
        barraT = barraT*barraT*(3-2*barraT);
        int xReveal = (int)(w * barraT);
        int y = i * barHeight;
        if (xReveal > 0)
        {
            DrawTexturePro(
                bg3,
                (Rectangle){0,
                            (float)(i*barHeight) * ((float)bg3.height/h),
                            xReveal * ((float)bg3.width/w),
                            barHeight * ((float)bg3.height/h)},
                (Rectangle){0,
                            y,
                            xReveal,
                            barHeight},
                (Vector2){0,0}, 0.0f, WHITE
            );
        }
    }
}
void DrawAnimacaoCarimboSobreFotos(float elapsed, int w, int h, Rectangle fotosRect, Texture2D carimbo)
{
    // ANIMAÇÃO DE CARIMBO (DANTE)
    // elapsed: tempo desde que a animação começou (em segundos)
    const float tempoQueda = 0.36f;
    const float tempoSquash = 0.13f;
    // const float tempoFade = 0.15f; // Não precisamos mais fazer o fade out

    const float squashScaleY = 0.67f; // quanto "achata" ao bater
    const float squashScaleX = 1.21f; // quanto "alarga" ao bater

    // Posição final pretendida (ex: centro da foto)
    float dstW = fotosRect.width / 5.0f;
    float dstH = fotosRect.height / 5.0f;
    float dstX = (fotosRect.x + (fotosRect.width - dstW) * 0.54f)-322; // leve desvio
    float dstY = (fotosRect.y + (fotosRect.height - dstH) * 0.54f)+100;
    // Você pode mudar a posição acima se quiser!

    float carimboW = carimbo.width;
    float carimboH = carimbo.height;

    // [1] FASE DE QUEDA
    float squashX = 1.0f;
    float squashY = 1.0f;
    //float angulo = -29.0f;
    float fadeIn = 1.0f;
    float yOff = 0;

    if (elapsed < tempoQueda) {
        // durante a queda...
        float t = elapsed / tempoQueda;
        // Da posição inicial (acima e girado) para o ponto do carimbo
        yOff = -fotosRect.height * (1.1f - t); // vem de cima
        //angulo = -50.0f + 21.0f * t;           // gira ao cair
        fadeIn = 0.1f + 0.89f * t;
    }
    else if (elapsed < tempoQueda + tempoSquash) {
        // [2] FASE DE IMPACTO (SQUASH)
        float t = (elapsed - tempoQueda) / tempoSquash;
        // squash e retorno suave à forma
        squashX = squashScaleX - (squashScaleX - 1) * t;
        squashY = squashScaleY + (1 - squashScaleY) * t; // volta para 1
        yOff = 0.f;
        fadeIn = 1.0f;
        //angulo = -29.0f + 6.0f * (1-t); // só um micro ajuste
    }
    else {
        // [3] PARADO
        squashX = 1;
        squashY = 1;
        yOff = 0.f;
        fadeIn = 1.0f;
        //angulo = -28.0f;
    }

    // *** NÃO faz fade out ao final! ***
    // if (elapsed > tempoQueda + tempoSquash + 1.7f) {
    //     float fadeT = Clamp((elapsed - tempoQueda - tempoSquash - 1.7f) / tempoFade, 0.0f, 1.0f);
    //     fadeIn = 1.0f - fadeT;
    // }

    unsigned char alpha = (unsigned char)(255 * fadeIn);

    // PREPARA RECTANGULO DE DESTINO (apply squash scale usando centro)
    float cx = dstX + dstW/2.0f;
    float cy = dstY + dstH/2.0f + yOff;
    Rectangle dst = { cx - (dstW*squashX)/2, cy - (dstH*squashY)/2, dstW*squashX, dstH*squashY };

    float angulo2 = 10.0f;
    DrawTexturePro(
        carimbo,
        (Rectangle){0,0,carimboW,carimboH},
        dst,
        (Vector2){dst.width/2, dst.height/2},
        angulo2,
        (Color){255,255,255,alpha}
    );
}
void DrawJadeAnim(float animTime, int w, int h)
{
    const float animDuration = 1.2f;
    if (animTime < 0.0f) animTime = 0.0f;
    if (animTime > animDuration) animTime = animDuration;
    float t = animTime / animDuration;
    float easeT = t * t * (3.0f - 2.0f * t);
    float spriteW = personagemJade.width;
    float spriteH = personagemJade.height;
    float escala = (h / 1.4f / spriteH);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    // Destino: canto inferior direito, 50px da borda direita
    float xFinal = w - dstW - 50.0f;
    float xStart = xFinal + 28.0f; // entra pela direita (da direita pra esquerda)
    float xAtual = xStart + (xFinal - xStart) * easeT;
    float yFinal = h - dstH - 20.0f; // 20px do rodapé
    unsigned char alpha = (unsigned char)(255 * easeT);
    if (alpha > 0)
    {
        DrawTexturePro(
            personagemJade,
            (Rectangle){0, 0, spriteW, spriteH},
            (Rectangle){xAtual, yFinal, dstW, dstH},
            (Vector2){0, 0},
            0.0f, (Color){255,255,255,alpha}
        );
    }
}
void DrawCutscenes(void)
{
    if (ended) return;
        if (aguardandoFimBgFinal) {
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        BeginDrawing();
        DrawTexturePro(
            bgFinal,
            (Rectangle){0, 0, bgFinal.width, bgFinal.height},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0}, 0.0f, WHITE
        );

        // --- ADICIONE AQUI ---
        float logoW = logo.width;
        float logoH = logo.height;
        // ESCALA: Reduz se quiser, ex: 1.0 = original, 0.5 = metade
        float tempo = GetTime();
        float escalaLogoBase = 1.0f;            // ESCALA BASE padrão da sua logo
        float intensidadePulse = 0.04f;          // Intensidade do pulso (quanto varia, 0.09 = ~9%)
        float velocidadePulse = 1.4f;            // Velocidade, 1.0 = um segundo por ciclo

        float escalaLogo = escalaLogoBase + intensidadePulse * sinf(tempo * velocidadePulse);

        float dstW = logoW * escalaLogo;
        float dstH = logoH * escalaLogo;
        float xLogo = w/2.0f - dstW/2.0f;
        float yLogo = (h/2.0f - dstH/2.0f)-200; // centralizado
        DrawTexturePro(
            logo,
            (Rectangle){0,0,logoW,logoH},
            (Rectangle){xLogo, yLogo, dstW, dstH},
            (Vector2){0,0},
            0.0f, WHITE
        );
        // Opcional: Mostra um texto de prompt
        DrawText("Pressione ESPAÇO para continuar", w/2-MeasureText("Pressione ESPAÇO para continuar",30)/2, h-80, 30, (Color){0,0,0,160});
        EndDrawing();
        return;
    }
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float time = GetTime() - startTime;
    BeginDrawing();
    if (time < 2.0f)
    {
        ClearBackground(BLACK);
    }
    else if (time < 5.8f)
    {
        float panT = (time - 2.0f) / 3.8f;
        if (panT > 1.0f) panT = 1.0f;
        float panEase = panT * panT * (3.0f - 2.0f * panT);
        float panPixels = w * 0.07f;
        float destX = panPixels * panEase;
        DrawTexturePro(
            empresa1,
            (Rectangle){0, 0, empresa1.width, empresa1.height},
            (Rectangle){destX, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        if (time < 3.4f)
        {
            float t = (time - 2.0f) / 1.4f;
            if (t > 1.0f) t = 1.0f;
            const int bars = 96;
            int barWidth = (w + bars - 1) / bars;
            for (int i = 0; i < bars; i++)
            {
                float barraInicio = (float)i / bars * 0.7f;
                float barraRandomOffset = (float)((i * 193) % 211) / 211.0f;
                barraInicio += barraRandomOffset * 0.09f;
                float barraT = (t - barraInicio) / 0.3f;
                if (barraT < 0) barraT = 0;
                if (barraT > 1) barraT = 1;
                barraT = barraT * barraT * (3.0f - 2.0f * barraT);
                int yReveal = (int)(h * barraT);
                DrawRectangle(i * barWidth, yReveal, barWidth, h - yReveal, BLACK);
            }
        }
    }
    else if (time < 5.93f)
    {
        ClearBackground(BLACK);
    }
    else if (time < 9.13f)
    {
        float showTime = time - 5.93f;
        float animDuration = 3.2f;
        float srcW = empresa2.width, srcH = empresa2.height;
        float zoomInicial = 2.3f;
        float centerYStart = 0.22f;
        float centerYEnd   = 0.5f;
        float centerX      = 0.5f;
        Rectangle srcRect;
        if (showTime < animDuration)
        {
            float t = showTime / animDuration;
            if (t > 1.0f) t = 1.0f;
            t = t * t * (3.0f - 2.0f * t);
            float currZoom = zoomInicial + (1.0f - zoomInicial) * t;
            float cropW = srcW / currZoom;
            float cropH = srcH / currZoom;
            float currCenterY = centerYStart + (centerYEnd - centerYStart) * t;
            float currCenterX = centerX;
            srcRect.x = srcW * currCenterX - cropW / 2.0f;
            srcRect.y = srcH * currCenterY - cropH / 2.0f;
            srcRect.width = cropW;
            srcRect.height = cropH;
            float fadeT = showTime / 0.29f;
            if (fadeT > 1.0f) fadeT = 1.0f;
            DrawTexturePro(
                empresa2,
                srcRect,
                (Rectangle){0, 0, w, h},
                (Vector2){0, 0},
                0.0f, WHITE
            );
            if (fadeT < 1.0f)
            {
                unsigned char fadeAlpha = (unsigned char)(255 * (1.0f - fadeT));
                DrawRectangle(0, 0, w, h, (Color){0, 0, 0, fadeAlpha});
            }
        }
        else
        {
            DrawTexturePro(
                empresa2,
                (Rectangle){0, 0, srcW, srcH},
                (Rectangle){0, 0, w, h},
                (Vector2){0, 0},
                0.0f, WHITE
            );
        }
    }
    else if (time < 9.31f)
    {
        ClearBackground(BLACK);
    }
    else if (time < 14.13f)
    {
        float panT = (time - 9.31f) / 4.82f;
        if (panT > 1.0f) panT = 1.0f;
        float panEase = panT * panT * (3.0f - 2.0f * panT);
        float srcW = empresa3.width;
        float srcH = empresa3.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropXmin = 0;
        float cropXmax = srcW - cropW;
        float cropX = cropXmax * (1.0f - panEase) + cropXmin * (panEase);
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa3,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
    else if (time < 14.63f)
    {
        float srcW = empresa3.width;
        float srcH = empresa3.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa3,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
    }
    else if (time < 15.83f)
    {
        float glitchT = (time - 14.63f) / (15.83f - 14.63f);
        if (glitchT > 1.0f) glitchT = 1.0f;
        float srcW = empresa3.width;
        float srcH = empresa3.height;
        float dstW = (float)w;
        float dstH = (float)h;
        float scaleX = srcW / dstW;
        float scaleY = srcH / dstH;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa3,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        int glitches = 18 + (int)(28 * glitchT);
        for (int i = 0; i < glitches; i++)
        {
            int y = GetRandomValue(0, h-5);
            int hline = GetRandomValue(1, 10);
            DrawRectangle(0, y, w, hline, BLACK);
            if (glitchT > 0.3f) {
                int rx = GetRandomValue(0, w - 60);
                int rw = GetRandomValue(16, 90);
                int ry = GetRandomValue(0, h - 20);
                int rh = GetRandomValue(8, 30);
                DrawRectangle(rx, ry, rw, rh, BLACK);
            }
        }
        if (glitchT > 0.7f) {
            unsigned char fadeAlpha = (unsigned char)(255 * (glitchT-0.7f) / 0.3f);
            if (fadeAlpha > 255) fadeAlpha = 255;
            DrawRectangle(0,0,w,h,(Color){0,0,0,fadeAlpha});
        }
    }
    else if (time < 16.3f)
    {
        float fadeInDuration = 0.47f;
        float timeSince = time - 15.83f;
        float fadeT = timeSince / fadeInDuration;
        if (fadeT > 1.0f) fadeT = 1.0f;
        if (fadeT < 0.0f) fadeT = 0.0f;
        float srcW = empresa4.width, srcH = empresa4.height;
        float scaleX = srcW / w;
        float scaleY = srcH / h;
        float scale = (scaleX < scaleY) ? scaleX : scaleY;
        float dstW = w, dstH = h;
        float cropW = dstW * scale;
        float cropH = dstH * scale;
        float cropX = 0;
        float cropY = (srcH - cropH) * 0.5f;
        DrawTexturePro(
            empresa4,
            (Rectangle){cropX, cropY, cropW, cropH},
            (Rectangle){0, 0, w, h},
            (Vector2){0, 0},
            0.0f, WHITE
        );
        unsigned char alpha = (unsigned char)(255 * (1.0f - fadeT));
        if (fadeT < 0.22f) {
            unsigned char flashAlpha = (unsigned char)(255 * (0.22f-fadeT)/0.22f);
            DrawRectangle(0,0,w,h, (Color){220,220,230,flashAlpha});
        } else {
            DrawRectangle(0,0,w,h,(Color){0,0,0,alpha});
        }
        float empresa4ShowTime = time - 15.83f;
        const float detetiveDur = 1.2f;
        const float tintaDelay = detetiveDur;
        if (fadeT >= 1.0f)
        {
            float detetiveAnimTime = empresa4ShowTime;
            if (detetiveAnimTime < 0.0f) detetiveAnimTime = 0.0f;
            if (detetiveAnimTime > detetiveDur) detetiveAnimTime = detetiveDur;
            DrawEmpresa4DetetiveAnim(detetiveAnimTime, w, h);
            if (empresa4ShowTime >= tintaDelay)
                DrawEmpresa4TintaAnimESelo(empresa4ShowTime - tintaDelay, w, h);
        }
    }
    else
    {
        float empresa4ShowTime = time - 16.3f;
        const float detetiveDur = 1.2f;
        const float tintaDelay = detetiveDur;
        const float extraCompany4Time = 1.6f;
        const float empresa4Duration = detetiveDur + tintaDelay + extraCompany4Time;
        if (!pinceladaIniciou && empresa4ShowTime > empresa4Duration) {
            pinceladaIniciou = true;
            pinceisStartTime = GetTime();
        }
        if (pinceladaIniciou && !pinceladaTerminou) {
            float pinceladaElapsed = GetTime() - pinceisStartTime;
            DrawAnimacaoPinceladaBg1(pinceladaElapsed, w, h);
        }
        else if (pinceladaTerminou) {
            float tempoBG1 = GetTime() - momentoBG1Inteiro;

            // --- INÍCIO DO BLOCO de Transição BG2 ---

            // Exibe bg1 e todos elementos (HANK, clones, nomeHank)
            DrawTexturePro(bg1,(Rectangle){0,0,bg1.width,bg1.height},(Rectangle){0,0,w,h},(Vector2){0,0},0.0f,WHITE);
            DrawHankAnim(tempoBG1, w, h);
            if (tempoBG1 > 1.2f)
                DrawV1HankAnim(tempoBG1 - 1.2f, w, h);
            if (tempoBG1 > 1.75f)
                DrawV2HankAnim(tempoBG1 - 1.75f, w, h);
            if (tempoBG1 > 2.20f)
                DrawV3HankAnim(tempoBG1 - 2.20f, w, h);
            if (tempoBG1 > 2.95f)
                DrawV4HankAnim(tempoBG1 - 2.95f, w, h);
            float nomeHankStart = 2.95f + 0.55f;
            if (tempoBG1 > nomeHankStart)
                DrawNomeHankAnim(tempoBG1 - nomeHankStart, w, h);

            // Após 2 segundos de bg1 cheio, inicia transição bg2
            float tempoAteTransicao = nomeHankStart + 1.3f;
            if (!bg2TransicaoIniciou && tempoBG1 > tempoAteTransicao)
            {
                bg2TransicaoIniciou = true;
                bg2TransicaoStartTime = GetTime();
            }

            // Se iniciou a transição, desenha transição (barras) para o bg2
            if (bg2TransicaoIniciou && !bg2TransicaoTerminou)
            {
                float elapsed = GetTime() - bg2TransicaoStartTime;
                DrawAnimacaoBarrasBg2(elapsed, w, h);
                if (elapsed > 1.23f)
                {
                    bg2TransicaoTerminou = true;
                    bg2MomentoTotal = GetTime();
                }
            }
            // Se terminou, desenha bg2 completo e encerra após pequeno delay
            else if (bg2TransicaoTerminou)
            {
                float elapsedBg2 = GetTime() - bg2MomentoTotal;
                // Iniciar animação alice após 0.20s do bg2
                if (!aliceApareceu && elapsedBg2 > 0.20f)
                {
                    aliceApareceu = true;
                    aliceStartTime = GetTime();
                    aliceAnimTerminou = false;
                }
                if (aliceApareceu && !aliceAnimTerminou)
                {
                    float animTime = GetTime() - aliceStartTime;
                    DrawTexturePro(bg2,(Rectangle){0,0,bg2.width,bg2.height},(Rectangle){0,0,w,h},(Vector2){0,0},0.0f,WHITE);
                    DrawAliceAnim(animTime, w, h);
                    if (animTime > 1.2f)
                        DrawV1AliceAnim(animTime - 1.2f, w, h);
                    if (animTime > 1.75f)
                        DrawV2AliceAnim(animTime - 1.75f, w, h);
                    if (animTime > 2.2f)
                        DrawV3AliceAnim(animTime - 2.2f, w, h);
                    float nomeAliceStart = 2.75f; // (1.2+0.55+0.45 aprox para v3 terminar)
                    if (animTime > nomeAliceStart)
                        DrawNomeAliceAnim(animTime - nomeAliceStart, w, h);
                    if (animTime > 1.2f)
                    {
                        aliceAnimTerminou = true;
                        aliceAnimTerminouTime = GetTime();
                    }
                }
                else if (aliceAnimTerminou)
                {
                    DrawTexturePro(bg2,(Rectangle){0,0,bg2.width,bg2.height},(Rectangle){0,0,w,h},(Vector2){0,0},0.0f,WHITE);
                    DrawAliceAnim(1.2f, w, h);
                    DrawV1AliceAnim(GetTime() - aliceAnimTerminouTime, w, h);
                    DrawV2AliceAnim(GetTime() - aliceAnimTerminouTime - 0.55f, w, h);
                    DrawV3AliceAnim(GetTime() - aliceAnimTerminouTime - 1.0f, w, h);
                    float nomeAliceStart = 1.0f; // ajuste tempo igual nomeHank, pode ser 1.0f~1.3f
                    float tempoDesdeTermino = GetTime() - aliceAnimTerminouTime;
                    if (tempoDesdeTermino > nomeAliceStart)
                        DrawNomeAliceAnim(tempoDesdeTermino - nomeAliceStart, w, h);
                    float tempoAposNomeAlice = 1.3f; // MESMO tempo extra usado na transição BG1->BG2
                    if (!bg3TransicaoIniciou && tempoDesdeTermino > nomeAliceStart + tempoAposNomeAlice) {
                        bg3TransicaoIniciou = true;
                        bg3TransicaoStartTime = GetTime();
                    }
                    if (bg3TransicaoIniciou && !bg3TransicaoTerminou) {
                        float elapsed = GetTime() - bg3TransicaoStartTime;
                        DrawAnimacaoBarrasBg3(elapsed, w, h);
                        if (elapsed > 1.23f) {
                            bg3TransicaoTerminou = true;
                            bg3MomentoTotal = GetTime();
                        }
                    } else if (bg3TransicaoTerminou) {
                        DrawTexturePro(
                            bg3,
                            (Rectangle){0,0,bg3.width,bg3.height},
                            (Rectangle){0,0,w,h},
                            (Vector2){0,0}, 0.0f, WHITE
                        );

                        float tempoLevi = GetTime() - bg3MomentoTotal;
                        float nomeLeviStart = 2.75f;
                        DrawLeviAnim(tempoLevi, w, h);
                        if (tempoLevi > 1.2f) DrawV1LeviAnim(tempoLevi - 1.2f, w, h);
                        if (tempoLevi > 1.75f) DrawV2LeviAnim(tempoLevi - 1.75f, w, h);
                        if (tempoLevi > 2.2f) DrawV3LeviAnim(tempoLevi - 2.2f, w, h);
                        if (tempoLevi > nomeLeviStart) DrawNomeLeviAnim(tempoLevi - nomeLeviStart, w, h);

                        if (!ended && tempoLevi > nomeLeviStart + 1.3f);
                        if (!bg4TransicaoIniciou && tempoLevi > nomeLeviStart + 1.3f) {
                            bg4TransicaoIniciou = true;
                            bg4TransicaoStartTime = GetTime();
                        }
                        if (bg4TransicaoIniciou && !bg4TransicaoTerminou) {
                            float elapsed = GetTime() - bg4TransicaoStartTime;
                            DrawAnimacaoBarrasBg4(elapsed, w, h);
                            if (elapsed > 1.23f) {
                                bg4TransicaoTerminou = true;
                                bg4MomentoTotal = GetTime();
                            }
                        } else if (bg4TransicaoTerminou) {
                            DrawTexturePro(
                                bg4,
                                (Rectangle){0,0,bg4.width,bg4.height},
                                (Rectangle){0,0,w,h},
                                (Vector2){0,0}, 0.0f, WHITE
                            );
                            float tempoDante = GetTime() - bg4MomentoTotal;
                            float nomeDanteStart = 2.75f;
                            DrawDanteAnim(tempoDante, w, h);
                            if (tempoDante > 1.2f) DrawV1DanteAnim(tempoDante - 1.2f, w, h);
                            if (tempoDante > 1.75f) DrawV2DanteAnim(tempoDante - 1.75f, w, h);
                            if (tempoDante > 2.2f) DrawV3DanteAnim(tempoDante - 2.2f, w, h);
                            if (tempoDante > nomeDanteStart) DrawNomeDanteAnim(tempoDante - nomeDanteStart, w, h);
                            if (!ended && tempoDante > nomeDanteStart + 1.3f);
                            if (!bg5TransicaoIniciou && tempoDante > nomeDanteStart + 1.3f) {
                                bg5TransicaoIniciou = true;
                                bg5TransicaoStartTime = GetTime();
                            }
                            if (bg5TransicaoIniciou && !bg5TransicaoTerminou) {
                                float elapsed = GetTime() - bg5TransicaoStartTime;
                                DrawAnimacaoBarrasBg5(elapsed, w, h);
                                if (elapsed > 1.23f) {
                                    bg5TransicaoTerminou = true;
                                    bg5MomentoTotal = GetTime();
                                }
                            } else if (bg5TransicaoTerminou) {
                                // desenha bg5 ANTES das animações da Jade
                                DrawTexturePro(
                                    bg5,
                                    (Rectangle){0,0,bg5.width,bg5.height},
                                    (Rectangle){0,0,w,h},
                                    (Vector2){0,0}, 0.0f, WHITE
                                );
                                float tempoJade = GetTime() - bg5MomentoTotal;
                                float nomeJadeStart = 2.75f;
                                // ANIMAÇÕES ACIMA DO BG5
                                DrawJadeAnim(tempoJade, w, h);
                                if (tempoJade > 1.2f)  DrawV1JadeAnim(tempoJade - 1.2f, w, h);
                                if (tempoJade > 1.75f) DrawV2JadeAnim(tempoJade - 1.75f, w, h);
                                if (tempoJade > 2.2f)  DrawV3JadeAnim(tempoJade - 2.2f, w, h);
                                if (tempoJade > nomeJadeStart)
                                    DrawNomeJadeAnim(tempoJade - nomeJadeStart, w, h);

                                // ---- NOVO: Após 1.3s do nome JADE, começa transição hacker glitch
                                if (!hackerTransicaoIniciou && tempoJade > nomeJadeStart + 1.3f) {
                                    hackerTransicaoIniciou = true;
                                    hackerTransicaoStartTime = GetTime();
                                }
                                // Se começou a transição, exibe animação de glitch
                                if (hackerTransicaoIniciou && !hackerTransicaoTerminou) {
                                    float elapsedHack = GetTime() - hackerTransicaoStartTime;
                                    float glitchT = elapsedHack / 2.3f;  // <--- aumente o tempo aqui!
                                    if (glitchT > 1.0f) glitchT = 1.0f;
                                    // suavizar para smoothstep
                                    glitchT = glitchT * glitchT * (3.0f - 2.0f * glitchT);
                                    DrawGlitchTransitionBg5ToHacker(glitchT, w, h);
                                    if (glitchT >= 1.0f) {
                                        hackerTransicaoTerminou = true;
                                        hackerMomentoTotal = GetTime();
                                    }
                                }
                                // Se terminou a transição, mostra bgHacker completo por pelo menos 1s
                                else if (hackerTransicaoTerminou) {
                                    DrawTexturePro(
                                        bgHacker,
                                        (Rectangle){0,0,bgHacker.width,bgHacker.height},
                                        (Rectangle){0,0,w,h},
                                        (Vector2){0,0}, 0.0f, WHITE
                                    );
                                    float timeSince = GetTime() - hackerMomentoTotal;

                                    float costasDur = 4.0f; // tempo mostrando as costas

                                    if (timeSince < costasDur) {
                                        DrawCostasSpritesOverBgHacker(timeSince, w, h);
                                    }
                                    else {
                                        // --- INÍCIO DA CENA bgSemiFinal ---
                                        if (!semiFinalIniciou) {
                                            semiFinalIniciou = true;
                                            semiFinalStartTime = GetTime();
                                            aliceComemoraAnimTerminou = false;
                                            fotosTodosApareceu = false;
                                        }
                                        float tempoSemi = GetTime() - semiFinalStartTime;

                                        // 1. Fundo: bgSemiFinal
                                        DrawTexturePro(
                                            bgSemiFinal,
                                            (Rectangle){0,0,bgSemiFinal.width,bgSemiFinal.height},
                                            (Rectangle){0,0,w,h},
                                            (Vector2){0,0}, 0.0f, WHITE
                                        );

                                        // 2. Animação AliceComemora (inferior esquerda)
                                        // Parâmetros de animação, ajuste conforme necessário
                                        const float aliceAnimIn   = 0.0f;
                                        const float aliceAnimDur  = 1.2f;

                                        float aliceW = aliceComemora.width;
                                        float aliceH = aliceComemora.height;
                                        float escAlice = (w * 0.200f) / aliceW;
                                        float dstWA = aliceW * escAlice;
                                        float dstHA = aliceH * escAlice;
                                        float margemBAIXO = 30.0f;
                                        float margemESQ   = 30.0f;

                                        if (!aliceComemoraAnimTerminou) {
                                            float animT = tempoSemi - aliceAnimIn;
                                            if (animT < 0.0f) animT = 0.0f;
                                            if (animT > aliceAnimDur) animT = aliceAnimDur;
                                            float t = animT / aliceAnimDur;
                                            float smoothT = t*t*(3-2*t);

                                            // Slide in + fade-in Alice
                                            float xStart = margemESQ - 70.0f;
                                            float xEnd   = margemESQ;
                                            float xAtual = xStart + (xEnd-xStart)*smoothT;
                                            float yEnd   = h - dstHA - margemBAIXO;
                                            unsigned char alpha = (unsigned char)(255 * smoothT);

                                            DrawTexturePro(
                                                aliceComemora,
                                                (Rectangle){0,0,aliceW,aliceH},
                                                (Rectangle){xAtual, yEnd, dstWA, dstHA},
                                                (Vector2){0,0}, 0.0f, (Color){255,255,255,alpha}
                                            );

                                            if (animT >= aliceAnimDur) {
                                                aliceComemoraAnimTerminou = true;
                                                aliceComemoraTerminoTime = GetTime();
                                            }
                                        } else {
                                            // Alice parada na posição final
                                            float xEnd   = margemESQ;
                                            float yEnd   = h - dstHA - margemBAIXO;
                                            DrawTexturePro(
                                                aliceComemora,
                                                (Rectangle){0,0,aliceW,aliceH},
                                                (Rectangle){xEnd, yEnd, dstWA, dstHA},
                                                (Vector2){0,0}, 0.0f, WHITE
                                            );
                                        }

                                        // 3. FotosTodos após Alice terminar + 0.5s (topo direita)
                                        // 3. FotosTodos após Alice terminar + 0.5s (topo direita)
                                        if (aliceComemoraAnimTerminou) {
                                            float tempoDesdeAliceFim = GetTime() - aliceComemoraTerminoTime;
                                            if (!fotosTodosApareceu && tempoDesdeAliceFim > 0.5f)
                                                fotosTodosApareceu = true;
                                            // RECOMENDÁVEL: adicionar um fade-in
                                            if (fotosTodosApareceu) {
                                                float fotosW = fotosTodos.width;
                                                float fotosH = fotosTodos.height;
                                                float escFotos = (w / fotosW) / 1.6;
                                                float dstWF = fotosW * escFotos;
                                                float dstHF = fotosH * escFotos;
                                                float margemTOPO = 34.0f;
                                                float margemDIREITA = 44.0f;
                                                // Fade-in
                                                float fadeFotos = tempoDesdeAliceFim - 0.5f;
                                                float alphaFotos = 255.0f;
                                                if (fadeFotos >= 0.0f && fadeFotos < 0.45f)
                                                    alphaFotos = 255.0f * (fadeFotos / 0.45f);
                                                if (alphaFotos > 255.0f) alphaFotos = 255.0f;
                                                Rectangle fotosRect = { w - dstWF - margemDIREITA, margemTOPO, dstWF, dstHF };
                                                DrawTexturePro(
                                                    fotosTodos,
                                                    (Rectangle){0, 0, fotosW, fotosH},
                                                    fotosRect,
                                                    (Vector2){0, 0}, 0.0f, (Color){255,255,255,(unsigned char)(alphaFotos)}
                                                );
                                                // ---- ANIMAÇÃO DE CARIMBO SOBRE FOTOS ----
                                                float startCarimbo = 0.32f;        // quando inicia o PRIMEIRO carimbo
                                                float intervaloCarimbo = 0.42f;    // intervalo de 0.42s entre cada um
                                                if (fadeFotos >= 0.37f) { // quando fotos já está quase cheia
                                                    // Dante
                                                    float tDante = fadeFotos - startCarimbo;
                                                    if (tDante < 0.0f) tDante = 0.0f;
                                                    if (fadeFotos >= (0.37f + 0*intervaloCarimbo))
                                                        DrawAnimacaoCarimboSobreFotos(tDante, w, h, fotosRect, rejDante);
                                                    // Jade
                                                    float tJade = fadeFotos - startCarimbo - 1*intervaloCarimbo;
                                                    if (tJade < 0.0f) tJade = 0.0f;
                                                    if (fadeFotos >= (0.37f + 1*intervaloCarimbo))
                                                        DrawAnimacaoCarimboSobreFotosJade(tJade, w, h, fotosRect, rejJade);
                                                    // Alice
                                                    float tAlice = fadeFotos - startCarimbo - 2*intervaloCarimbo;
                                                    if (tAlice < 0.0f) tAlice = 0.0f;
                                                    if (fadeFotos >= (0.37f + 2*intervaloCarimbo))
                                                        DrawAnimacaoCarimboSobreFotosAlice(tAlice, w, h, fotosRect, rejAlice);
                                                    // Levi -- SOMENTE UMA DECLARAÇÃO!!!
                                                    float tLevi = fadeFotos - startCarimbo - 3*intervaloCarimbo;
                                                    if (tLevi < 0.0f) tLevi = 0.0f;
                                                    if (fadeFotos >= (0.37f + 3*intervaloCarimbo))
                                                        DrawAnimacaoCarimboSobreFotosLevi(tLevi, w, h, fotosRect, rejLevi);

                                                    // ---- INÍCIO DA LÓGICA DAS PISCADAS ----
                                                    if (!animPiscadasComecou && tLevi > 0.7f) {
                                                        animPiscadasComecou = true;
                                                        animPiscadasStartTime = GetTime();
                                                        estadoPiscadaAtual = 1; // primeira piscada
                                                    }

                                                    if (animPiscadasComecou && estadoPiscadaAtual <= 3) {
                                                        float tPiscada = GetTime() - animPiscadasStartTime;
                                                        // Durações das piscadas
                                                        const float dur1 = 0.8f;
                                                        const float durMeio1 = 0.17f; // tempo “olho fechado” antes de abrir de novo
                                                        const float dur2 = 0.6f;
                                                        const float durMeio2 = 0.14f;
                                                        const float dur3 = 0.4f;
                                                        const float durMeio3 = 0.19f;
                                                        float t = tPiscada; // tempo relativo

                                                        if (estadoPiscadaAtual == 1) { // BLINK 1
                                                            // Fechar
                                                            if (t < dur1)
                                                                DrawPiscadaOlho(t/dur1, w, h);
                                                            // Fica fechado um pouco
                                                            else if (t < dur1 + durMeio1)
                                                                DrawPiscadaOlho(1.0f, w, h);
                                                            // Abrindo de volta...
                                                            else if (t < dur1 + durMeio1 + dur1)
                                                                DrawPiscadaOlho(1.0f - ((t - (dur1+durMeio1))/dur1), w, h);
                                                            else {
                                                                estadoPiscadaAtual = 2;
                                                                animPiscadasStartTime = GetTime();
                                                            }
                                                        }
                                                        else if (estadoPiscadaAtual == 2) { // BLINK 2 (mais rápido)
                                                            t = tPiscada;
                                                            if (t < dur2)
                                                                DrawPiscadaOlho(t/dur2, w, h);
                                                            else if (t < dur2 + durMeio2)
                                                                DrawPiscadaOlho(1.0f, w, h);
                                                            else if (t < dur2 + durMeio2 + dur2)
                                                                DrawPiscadaOlho(1.0f - ((t - (dur2+durMeio2))/dur2), w, h);
                                                            else {
                                                                estadoPiscadaAtual = 3;
                                                                animPiscadasStartTime = GetTime(); // Próxima!
                                                            }
                                                        }
                                                        else if (estadoPiscadaAtual == 3) { // BLINK 3 (mais rápido; ao abrir, fica branco)
                                                            if (!mostrandoBgFinal) mostrandoBgFinal = true;
                                                            t = tPiscada;
                                                            if (t < dur3)
                                                                DrawPiscadaOlho(t/dur3, w, h);
                                                            else if (t < dur3 + durMeio3)
                                                                DrawPiscadaOlho(1.0f, w, h);
                                                            else if (t < dur3 + durMeio3 + dur3) {
                                                                // ABRE O OLHO MOSTRANDO O ZOOM-OUT NO BGFINAL!
                                                                float tAbre = (t - (dur3+durMeio3))/dur3;
                                                                tAbre = Clamp(tAbre, 0.0f, 1.0f);

                                                                // Inicia flag de animação de zoom (segurança, caso use normal)
                                                                if (!animZoomFinalIniciou) {
                                                                    animZoomFinalIniciou = true;
                                                                    animZoomFinalStart = GetTime();
                                                                    animZoomFinalTerminou = false;
                                                                }

                                                                // --- Chame o efeito ---
                                                                DrawZoomOutBgFinal(tAbre, w, h);
                                                                // --- ADICIONE O LOGO AQUI TAMBÉM ---
                                                                float logoW = logo.width;
                                                                float logoH = logo.height;
                                                                float escalaLogo = 0.47f;
                                                                float dstW = logoW * escalaLogo;
                                                                float dstH = logoH * escalaLogo;
                                                                float xLogo = w/2.0f - dstW/2.0f;
                                                                float yLogo = h/2.0f - dstH/2.0f;
                                                                DrawTexturePro(
                                                                    logo,
                                                                    (Rectangle){0,0,logoW,logoH},
                                                                    (Rectangle){xLogo, yLogo, dstW, dstH},
                                                                    (Vector2){0,0},
                                                                    0.0f, WHITE
                                                                );
                                                                // Pálpebra cobrindo (preto total até abrir)
                                                                DrawPiscadaOlho(1.0f - tAbre, w, h);
                                                            }
                                                            else { // ACABOU, FICA PRESO ATÉ BARRA DE ESPAÇO
                                                                estadoPiscadaAtual = 4;
                                                                animPiscadasComecou = false;
                                                                aguardandoFimBgFinal = true;
                                                            }
                                                        }
                                                        // IMPORTANTE: As piscadas sobrepõem as imagens! Só desenhe por cima do resto.
                                                    }
                                                    // ---- FIM LÓGICA DAS PISCADAS ----
                                                }
                                            }
                                        }

                                        // 4. Para finalizar, só termina com um clique/tecla ou tempo máximo na tela...
                                        // (opcional, dependendo do seu fluxo; caso queira forçar fim após X seg:)
                                        // if (tempoSemi > 7.0f) ended = true;

                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    // Enquanto não começou Alice, só desenha o bg2
                    DrawTexturePro(
                        bg2,
                        (Rectangle){0,0,bg2.width,bg2.height},
                        (Rectangle){0,0,w,h},
                        (Vector2){0,0}, 0.0f, WHITE
                    );
                }
            }
            // Se nem iniciou transição e passou tempo, termina forçadamente
            else if (!bg2TransicaoIniciou && tempoBG1 > nomeHankStart + 1.15f + 2.0f )
            {
                ended = true;
            }
            // -- FIM bloco MOD BG2 TRANSIÇÃO --
        }
        else {
            float srcW = empresa4.width;
            float srcH = empresa4.height;
            float dstW = (float)w;
            float dstH = (float)h;
            float scaleX = srcW / dstW;
            float scaleY = srcH / dstH;
            float scale = (scaleX < scaleY) ? scaleX : scaleY;
            float cropW = dstW * scale;
            float cropH = dstH * scale;
            float cropX = 0;
            float cropY = (srcH - cropH) * 0.5f;
            DrawTexturePro(
                empresa4,
                (Rectangle){cropX, cropY, cropW, cropH},
                (Rectangle){0, 0, w, h},
                (Vector2){0, 0},
                0.0f, WHITE
            );
            float detetiveAnimTime = empresa4ShowTime;
            if (detetiveAnimTime < 0.0f) detetiveAnimTime = 0.0f;
            if (detetiveAnimTime > detetiveDur) detetiveAnimTime = detetiveDur;
            DrawEmpresa4DetetiveAnim(detetiveAnimTime, w, h);
            if (empresa4ShowTime >= tintaDelay)
                DrawEmpresa4TintaAnimESelo(empresa4ShowTime - tintaDelay, w, h);
        }
    }
    EndDrawing();
}

bool CutscenesEnded(void)
{
    return ended;
}

void UnloadCutscenes(void)
{
    UnloadTexture(empresa1);
    UnloadTexture(empresa2);
    UnloadTexture(empresa3);
    UnloadTexture(empresa4);
    UnloadTexture(detetiveNaEmpresa4);
    UnloadTexture(minhaTinta);
    UnloadTexture(meuSelo);
    UnloadTexture(bg1);
    UnloadTexture(bg2); // BG2 NOVO!
    UnloadTexture(personagemHank);
    UnloadTexture(v1_hank);
    UnloadTexture(v2_hank);
    UnloadTexture(v3_hank);
    UnloadTexture(v4_hank);
    UnloadTexture(nomeHank);
    UnloadTexture(personagemAlice);
    UnloadTexture(v1_alice);
    UnloadTexture(v2_alice);
    UnloadTexture(v3_alice);
    UnloadTexture(nomeAlice);
    UnloadTexture(bg3);
    UnloadTexture(personagemLevi);
    UnloadTexture(v1_levi);
    UnloadTexture(v2_levi);
    UnloadTexture(v3_levi);
    UnloadTexture(nomeLevi);
    UnloadTexture(bg4);
    UnloadTexture(personagemDante);
    UnloadTexture(v1_dante);
    UnloadTexture(v2_dante);
    UnloadTexture(v3_dante);
    UnloadTexture(nomeDante);
    UnloadTexture(bg5);
    UnloadTexture(personagemJade);
    UnloadTexture(v1_jade);
    UnloadTexture(v2_jade);
    UnloadTexture(v3_jade);
    UnloadTexture(nomeJade);
    UnloadTexture(bgHacker);
    UnloadTexture(costasJade);
    UnloadTexture(costasLevi);
    UnloadTexture(costasAlice);
    UnloadTexture(costasDante);
    UnloadTexture(bgSemiFinal);
    UnloadTexture(aliceComemora);
    UnloadTexture(fotosTodos);
    UnloadTexture(rejDante);
    UnloadTexture(rejJade);
    UnloadTexture(rejAlice);
    UnloadTexture(rejLevi);
    UnloadTexture(bgFinal);
    UnloadTexture(logo);
}