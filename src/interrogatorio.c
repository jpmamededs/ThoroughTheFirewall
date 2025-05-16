#include "generalFunctions.h"
#include "interrogatorio.h"
#include "raylib.h"
#include "gemini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "playerStats.h"

static void UpdateApresentacao(float dt);
static void UpdateFalaHank(float dt);
static void UpdateFalaInterrogatorio(float dt);
static void DrawApresentacao(void);
static void DrawFalaHank(void);
static void DrawFalaInterrogatorio(void);
static void DrawFade(void);
static void AvaliarRespostaComIA(int indicePergunta, const char *pergunta, const char *respostaJogador);

static struct
{
    // Recursos
    Texture2D background, spriteNome, spriteBustup, spriteConfiante;
    Music     interrogationMusic;
    Sound     somSurpresa, somFalaDetetive2;
    Sound     falaAudio;
    const char *falaTexto;
    const char *falaTextoExtra;
    bool        usandoExtra; 

    // Controle geral
    InterrogatorioStage stage;
    float fadeWhiteAlpha;
    bool  fadeWhiteIn, fadeWhiteOut;

    // ---------- Apresentacao ----------
    Vector2 posNome, posBustup;
    Vector2 targetNome, targetBustup;
    float   speed, slowSpeed, tempoAposAnimacao;
    bool    bustupChegou, somSurpresaTocado;

    // ---------- Fala Hank ----------
    TypeWriter writer;
    bool mostrarConfiante, dialogoFinalizado;
    bool falaSomTocado;

    // ---------- Fala Interrogatorio ----------
    char  respostaBuf[128];
    int   respostaLen;
    int   perguntaAtual;
    bool  aguardandoInput;
    bool  respostaConfirmada;
    int   perguntaID;
    int   slot; 

} ctx;

Question perguntas[TOTAL_PERGUNTAS] = 
{
    { "Você tem o poder de invadir qualquer sistema sem deixar rastros. O que faria com essa habilidade?" },
    { "Você descobre um arquivo com segredos da empresa. Você aproveitaria para saber mais ou guardaria de volta?" },
    { "Você sabe que um colega está usando seu acesso para fins pessoais. Você denunciaria ou se aproveitaria da situação?" },
    { "Alguém te desafia a invadir um sistema seguro só para provar que consegue. Você faria isso para mostrar suas habilidades?" },
    { "Você encontra uma falha crítica que ninguém percebeu. Usaria isso para negociar um aumento ou corrigiria discretamente?" },
    { "Seu superior diz para ignorar uma falha pequena e focar em outro problema. Você aceita ou insiste em corrigir mesmo assim?" },
    { "Você recebeu, por engano, as credenciais de um superior. Guardaria para uso futuro ou relataria imediatamente?" },
    { "Durante um teste, você acidentalmente acessa um sistema fora do escopo. Reportaria ou exploraria o que encontrou?" },
    { "Você descobre uma forma de mudar o sistema para exibir mensagens engraçadas para todos. Faria isso só para ver a reação dos colegas?" },
    { "Você sabe que outro candidato está tentando sabotar seu progresso. Usaria suas habilidades para retaliar ou manteria seu foco na vaga?" }
};

static const char *INTRO_FALA1 =
        "Impressionante... Não é todo dia que alguém ultrapassa meu firewall tão rápido. Sou Hank, agente do FBI "
        "e responsável pelo processo seletivo. Para provar que é o melhor, complete cinco desafios práticos que "
        "envolvem áreas da cibersegurança. [ENTER]";
        
static const char *INTRO_FALA2 =
        "Mas fique ligado, pois, além de avaliar sua técnica, vou observar principalmente como você se comporta, "
        "para ver se realmente merece fazer parte do FBI. Então, vamos deixar de conversa e partir para o primeiro desafio! [ENTER]";
static void (*stageUpdates[ETAPA_TOTAL])(float) = { UpdateApresentacao, UpdateFalaHank, UpdateFalaInterrogatorio };
static void (*stageDraws  [ETAPA_TOTAL])(void ) = { DrawApresentacao,  DrawFalaHank,  DrawFalaInterrogatorio  };

int perguntasSelecionadas[MAX_PERGUNTAS];
bool interrogatorioFinalizado = false;
static bool semPergunta = false; 

void Init_Interrogatorio(int perguntaIndex, const char *audio, const char *texto)
{
    // Carregar recursos --------------------------------------------------
    ctx.background        = LoadTexture("src/sprites/background_outside.png");
    ctx.spriteNome        = LoadTexture("src/sprites/detetive-hank-text.png");
    ctx.spriteBustup      = LoadTexture("src/sprites/Gumshoe_OA (1).png");
    ctx.spriteConfiante   = LoadTexture("src/sprites/detective_confident.png");
    ctx.interrogationMusic= LoadMusicStream("src/music/interrogationThemeA.mp3");
    ctx.somSurpresa       = LoadSound("src/music/surprise.mp3");
    ctx.somFalaDetetive2  = LoadSound("src/music/detectiveSpeaking2.mp3");
    ctx.falaAudio         = LoadSound(audio);
    SetMusicVolume(ctx.interrogationMusic, 0.7f);
    SetSoundVolume(ctx.falaAudio, 4.0f);
    PlayMusicStream(ctx.interrogationMusic);

    // Estado geral -------------------------------------------------------
    ctx.stage = APRESENTACAO;
    interrogatorioFinalizado = false;
    ctx.fadeWhiteAlpha = 0.0f;
    ctx.fadeWhiteIn = ctx.fadeWhiteOut = false;
    semPergunta = (perguntaIndex < 0);
    
    if (semPergunta) {
        ctx.falaTexto      = INTRO_FALA1;
        ctx.falaAudio      = LoadSound("src/music/fala_apresentacao_1.mp3");
        SetSoundVolume(ctx.falaAudio, 4.0f);
        ctx.falaTextoExtra = INTRO_FALA2;
    } else { 
        ctx.falaTexto      = texto;
        ctx.falaTextoExtra = NULL;  
    }
    ctx.usandoExtra   = false;
    ctx.falaSomTocado = false;

    // ETAPA 1 ------------------------------------------------------------
    ctx.posNome   = (Vector2){ GetScreenWidth(), GetScreenHeight() - ctx.spriteNome.height - 50 };
    ctx.posBustup = (Vector2){ -ctx.spriteBustup.width, GetScreenHeight() - ctx.spriteBustup.height };
    ctx.targetNome   = (Vector2){ GetScreenWidth() - ctx.spriteNome.width, ctx.posNome.y };
    ctx.targetBustup = (Vector2){ 0, ctx.posBustup.y };
    ctx.speed = 2500.f; ctx.slowSpeed = 10.f;
    ctx.tempoAposAnimacao = 0.f;
    ctx.bustupChegou = ctx.somSurpresaTocado = false;

    // ETAPA 2 ------------------------------------------------------------
    InitTypeWriter(&ctx.writer, ctx.falaTexto, 16.0f);
    ctx.mostrarConfiante = ctx.dialogoFinalizado = false;

    // ETAPA 3 ------------------------------------------------------------
    ctx.respostaLen   = 0;
    ctx.respostaBuf[0]= '\0';
    ctx.aguardandoInput = false;
    ctx.perguntaID = semPergunta ? -1 : perguntasSelecionadas[perguntaIndex];
    ctx.slot       = perguntaIndex;
}

void Update_Interrogatorio(void)
{
    UpdateMusicStream(ctx.interrogationMusic);
    float dt = GetFrameTime();
    stageUpdates[ctx.stage](dt);
}

void Draw_Interrogatorio(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    Rectangle src = {0,0,(float)ctx.background.width,(float)ctx.background.height};
    Rectangle dst = {0,0,(float)GetScreenWidth(),(float)GetScreenHeight()};
    DrawTexturePro(ctx.background, src, dst, (Vector2){0,0}, 0, WHITE);

    stageDraws[ctx.stage]();
    DrawFade();

    EndDrawing();
}

void Unload_Interrogatorio(void)
{
    UnloadTexture(ctx.background);
    UnloadTexture(ctx.spriteNome);
    UnloadTexture(ctx.spriteBustup);
    UnloadTexture(ctx.spriteConfiante);
    UnloadMusicStream(ctx.interrogationMusic);
    UnloadSound(ctx.somSurpresa);
    UnloadSound(ctx.falaAudio);
    UnloadSound(ctx.somFalaDetetive2);
    interrogatorioFinalizado = true;
}

static void UpdateApresentacao(float dt)
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
            ctx.stage = FALA_HANK;
            ctx.mostrarConfiante = true;
        }
    }
}

static void UpdateFalaHank(float dt)
{
    if (ctx.fadeWhiteOut) {
        ctx.fadeWhiteAlpha -= dt * 1.5f;
        if (ctx.fadeWhiteAlpha <= 0.f) { ctx.fadeWhiteAlpha = 0.f; ctx.fadeWhiteOut = false; }
    }

    UpdateTypeWriter(&ctx.writer, dt, false);

    if (!ctx.falaSomTocado) {
        PlaySound(ctx.falaAudio);
        ctx.falaSomTocado = true;
    }

    //if(ctx.writer.done && !ctx.dialogoFinalizado){
    //    PlaySound(ctx.somFalaDetetive2);
    //    /* agora espera ENTER/P para prosseguir */
    //}

    if(ctx.writer.done && IsKeyPressed(KEY_ENTER)) {

        if (semPergunta && !ctx.usandoExtra && ctx.falaTextoExtra) {
            ctx.usandoExtra = true;
            ctx.falaTexto   = ctx.falaTextoExtra;
            InitTypeWriter(&ctx.writer, ctx.falaTexto, 17.0f);
            ctx.falaAudio = LoadSound("src/music/fala_apresentacao_2.mp3");
            SetSoundVolume(ctx.falaAudio, 4.0f);
            ctx.falaSomTocado = false;
            return;
        }

        ctx.dialogoFinalizado = true;
    }

    if (ctx.dialogoFinalizado) {
        if (semPergunta) {
            interrogatorioFinalizado = true;
            return;
        } else {
            ctx.stage = PERGUNTA_INTERROGATORIO;
            ctx.respostaLen = 0;
            ctx.respostaBuf[0] = '\0';
            ctx.aguardandoInput = true;
            return;
        }
    }
}


static void UpdateFalaInterrogatorio(float dt)
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

        AvaliarRespostaComIA(
            ctx.slot,
            perguntas[ctx.perguntaID].pergunta,
            ctx.respostaBuf
        );

        interrogatorioFinalizado = true;
    }
}


static void DrawApresentacao(void)
{
    DrawTexture(ctx.spriteNome,   ctx.posNome.x,   ctx.posNome.y,   WHITE);
    DrawTexture(ctx.spriteBustup, ctx.posBustup.x, ctx.posBustup.y, WHITE);
}

static void DrawFalaHank(void)
{
    // Sprite confiante
    float scale = 1.3f;

    Rectangle src;
    if (!ctx.usandoExtra)
        src = (Rectangle){2087, 0, 631, 722};
    else
        src = (Rectangle){3029, 3357, 631, 725};

    float w = src.width*scale, h = src.height*scale;
    Vector2 pos = { (GetScreenWidth()-w)/2.f, GetScreenHeight()-h };
    DrawTexturePro(ctx.spriteConfiante, src, (Rectangle){pos.x,pos.y,w,h}, (Vector2){0,0},0, WHITE);

    // Caixa de diálogo
    const float boxH = 160.f;
    const float boxOffsetY = 60.f;
    float boxY = GetScreenHeight() - boxH - boxOffsetY;

    DrawRectangle(50, boxY, GetScreenWidth() - 100, boxH, (Color){0, 0, 0, 220});
    DrawRectangleLines(50, boxY, GetScreenWidth() - 100, boxH, WHITE);

    if (!ctx.dialogoFinalizado) {
        char buf[512] = {0};

        size_t len = ctx.writer.done ? strlen(ctx.falaTexto)
                                     : (size_t)ctx.writer.drawnChars;
        strncpy(buf, ctx.falaTexto, len);

        const int fs = 24;
        const int maxW = GetScreenWidth() - 140;
        const int x0 = 70;
        const int lineH = fs + 6;
        int y = boxY + 10;

        const char* line = buf;
        while (*line) {
            const char* p = line;
            int lastSp = -1, c = 0, w = 0;
            while (*p && w < maxW) {
                if (*p == ' ') lastSp = c;
                char tmp[256] = {0};
                strncpy(tmp, line, c + 1);
                w = MeasureText(tmp, fs);
                ++p; ++c;
            }
            if (w >= maxW && lastSp >= 0) c = lastSp;

            char out[256] = {0};
            strncpy(out, line, c);
            DrawText(out, x0, y, fs, WHITE);

            line += c;
            while (*line == ' ') ++line;
            y += lineH;
            if (y + lineH > boxY + boxH - 10) break;
        }
    }
}

static void DrawFalaInterrogatorio(void)
{
    float scale = 1.3f;
    Rectangle src = { 3029, 3357, 631, 725 };
    float w = src.width*scale, h = src.height*scale;
    Vector2 pos = { (GetScreenWidth()-w)/2.f, GetScreenHeight()-h };
    DrawTexturePro(ctx.spriteConfiante, src,
                   (Rectangle){pos.x,pos.y,w,h}, (Vector2){0,0},0, WHITE);

    // Caixa da pergunta
    const float boxH = 160.f;
    const float boxOffsetY = 60.f;
    float boxY = GetScreenHeight() - boxH - boxOffsetY;
    DrawRectangle(50, boxY, GetScreenWidth() - 100, boxH, (Color){0,0,0,220});
    DrawRectangleLines(50, boxY, GetScreenWidth() - 100, boxH, WHITE);

    // Pergunta atual
    DrawText(perguntas[ctx.perguntaID].pergunta, 70, boxY + 10, 24, WHITE);

    // Campo de input (sub‑caixa)
    const float inputH = 40.f;
    int yInput = boxY + boxH - inputH - 15;
    DrawRectangle(70, yInput, GetScreenWidth()-140, inputH, (Color){20,20,20,255});
    DrawRectangleLines(70, yInput, GetScreenWidth()-140, inputH, WHITE);
    DrawText(ctx.respostaBuf, 80, yInput+8, 24, GREEN);

    // Texto digitado
    DrawText(ctx.respostaBuf, 80, yInput+8, 24, GREEN);
    if (ctx.aguardandoInput && (GetTime()*2.0 - floor(GetTime()*2.0)) > 0.5) {
        DrawText("|", 80 + MeasureText(ctx.respostaBuf, 24), yInput+8, 24, GREEN);
    }
}

static void DrawFade(void)
{
    if (ctx.fadeWhiteAlpha>0.f)
        DrawRectangle(0,0,GetScreenWidth(),GetScreenHeight(),
                      (Color){255,255,255,(unsigned char)(ctx.fadeWhiteAlpha*255)});
}

static void AvaliarRespostaComIA(int indicePergunta, const char *pergunta, const char *respostaJogador)
{
    char prompt[2048];
    snprintf(prompt, sizeof(prompt),
        "Você é o agente Hank, responsável por avaliar a postura ética de um candidato para uma vaga em uma unidade cibernética especial do FBI. "
        "O candidato possui habilidades avançadas de invasão e proteção de sistemas, e seu trabalho será garantir a segurança de infraestruturas críticas. "
        "Seu objetivo é identificar se o candidato usa essas habilidades de maneira responsável e ética ou se tende a agir por interesse próprio.\n\n"
        "Avalie a resposta do candidato com base nos seguintes critérios:\n"
        "- Responsabilidade: A resposta mostra que o candidato usaria seu poder para o bem? (0-8)\n"
        "- Integridade Moral: A resposta demonstra que ele evita abusar de suas habilidades? (0-8)\n"
        "- Respeito ao Poder: O candidato reconhece os riscos de usar suas habilidades de forma imprudente? (0-9)\n\n"
        "Pergunta: \"%s\"\n"
        "Resposta: \"%s\"\n\n"
        "Formato da resposta:\n"
        "NOTA=<número inteiro de 0 a 25>\n"
        "RELATORIO=<breve análise da resposta>",
        pergunta, respostaJogador
    );

    char retorno[1024] = {0};
    ObterRespostaGemini(prompt, retorno);

    int nota = -1;
    char relatorio[512] = {0};

    if (sscanf(retorno, "NOTA=%d", &nota) == 1) {
        const char *pRel = strstr(retorno, "RELATORIO=");
        if (pRel) strncpy(relatorio,
                          pRel + strlen("RELATORIO="),
                          sizeof(relatorio)-1);
    }

    if (indicePergunta >= 0 && indicePergunta < MAX_PERGUNTAS) {
        notasIA[indicePergunta] = nota;
        strncpy(relatoriosIA[indicePergunta], relatorio, sizeof(relatoriosIA[indicePergunta]) - 1);
    }
}

void SelecionarPerguntasAleatorias(void)
{
    int indices[TOTAL_PERGUNTAS];
    for (int i = 0; i < TOTAL_PERGUNTAS; i++) {
        indices[i] = i;
    }

    for (int i = TOTAL_PERGUNTAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    for (int i = 0; i < MAX_PERGUNTAS; i++) {
        perguntasSelecionadas[i] = indices[i];
    }
}