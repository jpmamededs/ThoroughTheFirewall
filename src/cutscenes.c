#include "raylib.h"
#include "cutscenes.h"
#include <math.h>

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
static int screenWidth, screenHeight;
static float startTime = 0.0f;
static bool ended = false;


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

static bool aliceApareceu = false;
static float aliceStartTime = 0.0f;
static bool aliceAnimTerminou = false;
static float aliceAnimTerminouTime = 0.0f;

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
}

void UpdateCutscenes(void)
{
    if (ended) return;
    float time = GetTime() - startTime;
    if (IsKeyPressed(KEY_SPACE)) ended = true;
    if (IsKeyPressed(KEY_SPACE) && time < 5.8f)
        startTime = GetTime() - 5.8f;
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
    const float animDuration = 0.55f;
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
    const float animDuration = 0.55f;
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
    const float animDuration = 0.55f;
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
// --- v1_hank animada próxima de Hank, menor e depois dele, e depois flutuando ---
void DrawV1HankAnim(float animTime, int w, int h)
{
    const float animDuration = 0.55f;
    float spriteW = v1_hank.width;
    float spriteH = v1_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 3.5f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala = escalaHank * (1.0f / 3.5f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xHank = 20.0f;
    float yHank = h - dstHH - 32.0f + 160.0f;
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
    const float animDuration = 0.55f;
    float spriteW = v2_hank.width;
    float spriteH = v2_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 3.5f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala = escalaHank * (1.0f / 1.6f);
    float dstW = spriteW * escala;
    float dstH = spriteH * escala;
    float xHank = 20.0f;
    float yHank = h - dstHH - 32.0f + 160.0f;
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
    const float animDuration = 0.55f;
    float spriteW2 = v2_hank.width;
    float spriteH2 = v2_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 3.5f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala2 = escalaHank * (1.0f / 1.4f);
    float dstW2 = spriteW2 * escala2;
    float dstH2 = spriteH2 * escala2;
    float xHank = 20.0f;
    float yHank = h - dstHH - 32.0f + 160.0f;
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
    const float animDuration = 0.55f;
    float spriteW2 = v2_hank.width;
    float spriteH2 = v2_hank.height;
    float escalaHank = (h / 4.0f / personagemHank.height) * 3.5f;
    float dstHW = personagemHank.width * escalaHank;
    float dstHH = personagemHank.height * escalaHank;
    float escala2 = escalaHank * (1.0f / 1.4f);
    float dstW2 = spriteW2 * escala2;
    float dstH2 = spriteH2 * escala2;
    float xHank = 20.0f;
    float yHank = h - dstHH - 32.0f + 160.0f;
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

void DrawCutscenes(void)
{
    if (ended) return;
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
            float tempoAteTransicao = nomeHankStart + 1.15f + 2.0f;
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
                    if (!ended && tempoDesdeTermino > nomeAliceStart + 2.0f)
                        ended = true;
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
}