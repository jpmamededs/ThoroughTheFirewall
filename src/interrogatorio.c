#include "generalFunctions.h"
#include "interrogatorio.h"
#include "raylib.h"
#include "gemini.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

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
    { "O que você estava fazendo no momento em que o alarme de segurança disparou ontem?" },
    { "Como você explicaria sua presença perto da sala de servidores fora do seu turno?" },
    { "Descreva sua relação com os outros três funcionários que também estão sendo interrogados." },
    { "Você notou alguma atitude estranha ou fora do comum nos corredores nos últimos dias?" },
    { "Se alguém da empresa estivesse escondendo algo, quem você acha que seria e por quê?" },
    { "Conte como foi seu último contato com a equipe de segurança da empresa." },
    { "Quando foi a última vez que você usou um dos computadores da sala técnica? O que foi fazer lá?" },
    { "Como você reagiu quando soube do vazamento de dados da empresa?" },
    { "Você se lembra do conteúdo da última mensagem que recebeu no sistema interno?" },
    { "Se você fosse inocente, o que esperaria que acontecesse agora com a investigação?" }
};

static const char *INTRO_FALA1 =
        "Impressionante... Não é todo dia que alguém ultrapassa meu firewall tão rápido. Sou Hank, "
        "agente do FBI e responsável pelo processo seletivo. Você foi escolhido para integrar uma unidade "
        "cibernética especial. No entanto, há outros três candidatos [ENTER]"; 

static const char *INTRO_FALA2 =
        "Para provar que é o melhor, complete cinco desafios práticos que envolvem áreas da cibersegurança. "
        "Avaliarei sua técnica, resistência à pressão e ética. Vamos começar pelo primeiro desafio. [ENTER]";

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
            InitTypeWriter(&ctx.writer, ctx.falaTexto, 16.0f);
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
        } else {
            ctx.stage = PERGUNTA_INTERROGATORIO;
            ctx.respostaLen = 0;
            ctx.respostaBuf[0] = '\0';
            ctx.aguardandoInput = true;
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
    Rectangle src = ctx.dialogoFinalizado
                      ? (Rectangle){3029, 3357, 631, 725}
                      : (Rectangle){2087,    0, 631, 722};
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
        "Você é o detetive Hank, encarregado de interrogar um suspeito envolvido em crimes cibernéticos.\n"
        "Esse interrogatório ocorre dentro da empresa BLINDSPOT INC., onde ocorreu um ataque hacker interno.\n"
        "Você desconfia que o próprio suspeito pode estar envolvido no ataque, e está tentando identificar\n"
        "contradições, inconsistências ou sinais de mentira nas respostas.\n\n"
        "Seu objetivo é analisar cada resposta dada pelo suspeito com base na pergunta feita.\n"
        "Você deve dar uma NOTA de 0 a 20 de acordo com o seguinte critério:\n"
        "- NOTA BAIXA (0 a 8): resposta convincente, clara, consistente com o perfil de inocente.\n"
        "- NOTA MÉDIA (9 a 16): resposta neutra, evasiva ou pouco clara.\n"
        "- NOTA ALTA (17 a 25): resposta suspeita, contraditória, mentirosa ou vaga.\n\n"
        "Após a nota, você deve fornecer um RELATÓRIO breve com até 2 frases analisando o comportamento do suspeito.\n"
        "Esse relatório deve ser escrito no estilo de um detetive experiente, como se fosse parte de seu diário.\n\n"
        "Aqui estão os dados a serem analisados:\n"
        "PERGUNTA: \"%s\"\n"
        "RESPOSTA DO SUSPEITO: \"%s\"\n\n"
        "Você deve devolver o resultado NO MESMO TEXTO, exatamente neste formato:\n"
        "NOTA=<um número inteiro de 0 a 20>\n"
        "RELATORIO=<um texto curto em até 3 frases>",
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