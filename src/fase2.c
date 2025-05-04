#include "fase2.h"
#include "raylib.h"
#include "menu.h"
#include "generalFunctions.h"
#include <string.h>
#include <stdio.h>
extern AppState state;
// Frases original, acerto e erro
#define FALA_NORMAL "Qual o primeiro passo que deve ser tomado para criar uma reverse shell em C99?"
#define FALA_ACERTO "Consegui lembrar. Acho que ninguem se quer esta pensando que eu possa ser o hacker! "
#define FALA_ERRO "Droga! Eu acho que fiz algo errado, devo ter disparado algum alerta, preciso fugir!"
// Recursos da fase 2
static Texture2D fundo;
static Texture2D pergunta_img;
// Sprites dos personagens
static Texture2D sprJoao, sprMateus, sprMateus2, sprMateus3, sprCarlos, sprMamede;
static Texture2D sprGemini;
static TypeWriter fase2Writer;
static bool typeStarted = false;
static bool podeAvancar = false;
// TIMER FASE2
static float fase2Timer = 30.0f;
static bool fase2TimerActive = true;
static bool fase2TimerFinished = false;
// NAVEGAÇÃO DAS OPÇÕES
static int selectedOption = 0;
static char fase2_fala[512] = FALA_NORMAL;
// --- Gemini Ajuda (balão canto superior esquerdo)
static bool geminiHelpClicked = false;
static const char* gemini_help_msg_default = "Clique aqui caso precise de ajuda!";
static const char* gemini_help_msg_ajuda = "A primeira, segunda e terceira opções estao erradas";
// --- Opções desabilitadas
static bool optionDisabled[5] = { false, false, false, false, false };
// --- Controle de desabilitação por timeout
static bool cronometroDesligado = false;
// Gemini box parameters (tornamos globais para recalcular no clique E em Draw)
static float geminiRectW = 550; // Inicial padrão
static float geminiRectH = 0;   // Calcular em tempo de execução
static int   geminiTextWidth = 0; // Atual em exibição
// GEMINI HELP ANIMATION
static float geminiRectAnim = 0.0f;     // 0.0 (off) até 1.0 (cheio)
static bool geminiMouseOver = false;
static float geminiAnimSpeed = 6.0f;
#define GEMINI_RECT_PADRAO 550
#define GEMINI_PAD_X 36
#define OPTION_TEXT_MARGIN_LEFT 36 // <<<<< MARGEM PARA O TEXTO DAS OPÇÕES
// --- NOVO: PARA O SISTEMA DE CORREÇÃO E PISCAR ---
static bool respostaRespondida = false;   // Se alguma alternativa já foi clicada
static int opcaoSelecionadaUsuario = -1;  // Qual alternativa foi clicada, -1 nenhuma
static float blinkTimer = 0.0f;           // Para o efeito de piscar
void AtualizaTamanhoGeminiBox() {
    float geminiScale = 0.1f;
    float geminiH = sprGemini.height * geminiScale;
    int txtSize = 20;
    const char* gemini_msg = !geminiHelpClicked ? gemini_help_msg_default : gemini_help_msg_ajuda;
    geminiTextWidth = MeasureText(gemini_msg, txtSize);
    float minW = GEMINI_RECT_PADRAO;
    float maxW = GetScreenWidth() - 140; // margem lateral
    geminiRectW = minW;
    if (geminiTextWidth + 2*GEMINI_PAD_X > minW)
        geminiRectW = (geminiTextWidth + 2*GEMINI_PAD_X > maxW) ? maxW : geminiTextWidth + 2*GEMINI_PAD_X;
    geminiRectH = geminiH * 0.75f;
}
void InitFase2(void)
{
    fundo = LoadTexture("src/sprites/empresa3.png");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    sprJoao    = LoadTexture("src/sprites/joaoSprite.png");
    sprMateus  = LoadTexture("src/sprites/mateusSprite.png");
    sprMateus2 = LoadTexture("src/sprites/mateus2.png");
    sprMateus3 = LoadTexture("src/sprites/mateus3.png");
    sprCarlos  = LoadTexture("src/sprites/carlosSprite.png");
    sprMamede  = LoadTexture("src/sprites/mamedeSprite.png");
    sprGemini  = LoadTexture("src/sprites/os/gemini.png");
    strcpy(fase2_fala, FALA_NORMAL);
    InitTypeWriter(&fase2Writer, fase2_fala, 18.5f);
    typeStarted = true;
    podeAvancar = false;
    fase2Timer = 30.0f;
    fase2TimerActive = true;
    fase2TimerFinished = false;
    selectedOption = 0;
    geminiRectAnim = 0.0f;
    geminiMouseOver = false;
    geminiHelpClicked = false;
    cronometroDesligado = false;
    for (int i = 0; i < 5; ++i) optionDisabled[i] = false; // habilite tudo ao iniciar
    AtualizaTamanhoGeminiBox();
    // -- sistema de resposta --
    respostaRespondida = false;
    opcaoSelecionadaUsuario = -1;
    blinkTimer = 0.0f;
}
void UpdateFase2(void)
{
    float delta = GetFrameTime();
    // GEMINI ANIMATION e hitbox
    float geminiX = 49, geminiY = 67, geminiScale = 0.1f;
    float geminiW = sprGemini.width * geminiScale;
    float geminiH = sprGemini.height * geminiScale;
    AtualizaTamanhoGeminiBox();
    float rx = geminiX + geminiW - 20.0f;
    float ry = geminiY + (geminiH - geminiRectH)/2.0f;
    Rectangle geminiLogoRec = {geminiX, geminiY, geminiW, geminiH};
    Rectangle geminiRectRec = {rx, ry, geminiRectW, geminiRectH};
    Vector2 mouseGem = GetMousePosition();
    bool mouseOverLogo = CheckCollisionPointRec(mouseGem, geminiLogoRec);
    bool mouseOverRect = CheckCollisionPointRec(mouseGem, geminiRectRec);
    geminiMouseOver = mouseOverLogo || mouseOverRect;
    float dir = geminiMouseOver ? 1.0f : -1.0f;
    geminiRectAnim += dir * geminiAnimSpeed * delta;
    if (geminiRectAnim > 1.0f) geminiRectAnim = 1.0f;
    if (geminiRectAnim < 0.0f) geminiRectAnim = 0.0f;
    if ((mouseOverLogo || mouseOverRect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        if (!geminiHelpClicked && !cronometroDesligado) {
            geminiHelpClicked = true;
            AtualizaTamanhoGeminiBox();
            geminiRectAnim = 1.0f;
            optionDisabled[0] = true;
            optionDisabled[1] = true;
            optionDisabled[2] = true;
            // também reposicione para opção habilitada mais próxima:
            if (optionDisabled[selectedOption]) {
                for (int i = 3; i < 5; i++)
                    if (!optionDisabled[i]) { selectedOption = i; break; }
            }
        }
    }
    // TIMER FASE2
    if (fase2TimerActive && !fase2TimerFinished) {
        fase2Timer -= delta;
        if (fase2Timer <= 0) {
            fase2Timer = 0;
            fase2TimerActive = false;
            fase2TimerFinished = true;
            for (int i = 0; i < 5; ++i) optionDisabled[i] = true;
            cronometroDesligado = true; // Marque que o timeout ocorreu!
        }
    }
    // -- VARIÁVEIS Q SÃO USADAS EM UPDATE E DRAW --
    int numRects = 5;
    int baseWidth = 600;
    int larguraStep = 85;
    int rectHeight = 78;
    int spacing = 28;
    int offsetY = 295;
    int offsetX = 35;
    // --- DETECTA CLIQUE E ENTER NAS OPÇÕES, SE PERMITIDO E NÃO RESPONDIDO ---
    // ENTER/RETURN (NÃO barra de espaço) seleciona a opção
    if (!respostaRespondida && !cronometroDesligado) {
        int i_clicada = -1;
        // Detecção pelo mouse
        for (int i = 0; i < numRects; i++) {
            int rectWidth = baseWidth + i * larguraStep;
            int y = offsetY + i * (rectHeight + spacing);
            int x = GetScreenWidth() - rectWidth - offsetX;
            Rectangle rec = {x, y, rectWidth, rectHeight};
            if (!optionDisabled[i] &&
                CheckCollisionPointRec(GetMousePosition(), rec) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                i_clicada = i;
            }
        }
        // Detecção ONLY por teclado: ENTER e KP_ENTER
        if (!optionDisabled[selectedOption] &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)))
        {
            i_clicada = selectedOption;
        }

        if (i_clicada != -1) {
            respostaRespondida = true;
            opcaoSelecionadaUsuario = i_clicada;
            blinkTimer = 0.0f;
            for (int j = 0; j < 5; j++) optionDisabled[j] = true;
            fase2TimerActive = false;
            cronometroDesligado = false;
            const char* name = MenuSelectedCharacterName();
            if (i_clicada == 4) {
                strcpy(fase2_fala, FALA_ACERTO);
                InitTypeWriter(&fase2Writer, fase2_fala, 18.5f);
                typeStarted = true;
                podeAvancar = false;
            } else if (name && strcmp(name, "Mateus") == 0) {
                strcpy(fase2_fala, FALA_ERRO);
                InitTypeWriter(&fase2Writer, fase2_fala, 18.5f);
                typeStarted = true;
                podeAvancar = false;
            }
        }
    }
    // NAVEGAÇÃO: só navega para opções habilitadas
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_UP)) {
        int dir = 0;
        if (IsKeyPressed(KEY_DOWN)) dir = 1;
        if (IsKeyPressed(KEY_UP))   dir = -1;
        int tries = 0;
        int trySel = selectedOption;
        do {
            trySel = (trySel + dir + numRects) % numRects;
            tries++;
        } while (optionDisabled[trySel] && tries < numRects);
        if (!optionDisabled[trySel])
            selectedOption = trySel;
    }
    if (typeStarted) {
        UpdateTypeWriter(&fase2Writer, delta, IsKeyPressed(KEY_SPACE));
        if (fase2Writer.drawnChars == (int)strlen(fase2_fala)) {
            podeAvancar = true;
        }
    }
    if (podeAvancar) {
        Rectangle btnBounds = {
            GetScreenWidth() / 2 - 100,
            GetScreenHeight() / 2 + 100,
            200,
            50};
        Vector2 mouse = GetMousePosition();
        if (CheckCollisionPointRec(mouse, btnBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            state = APP_PC_SCREEN;
        }
    }
}
void DrawFase2(void)
{
    float delta = GetFrameTime();
    blinkTimer += delta;
    BeginDrawing();
    ClearBackground(RAYWHITE);
    // FUNDO
    DrawTextureEx(fundo, (Vector2){0,0}, 0.0f,
                  (float)GetScreenWidth() / fundo.width, RAYWHITE);
    // Caixa de fala e imagem da pergunta
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;
    DrawTexturePro(pergunta_img, (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
                   (Rectangle){imgX, imgY, imgW, imgH}, (Vector2){0, 0}, 0.0f, WHITE);
    // ==== ALTERAÇÃO PARA EXIBIR O NOME DO PERSONAGEM ====
    const char* name = MenuSelectedCharacterName();
    DrawText((name && name[0]) ? name : "???", imgX + 10, imgY + imgH - 30, 30, WHITE);
    int borderRadius = boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});
    if (fase2Writer.drawnChars > 0) {
        char tmp[fase2Writer.drawnChars + 1];
        strncpy(tmp, fase2_fala, fase2Writer.drawnChars);
        tmp[fase2Writer.drawnChars] = '\0';
        DrawText(tmp, boxX + 20, boxY + 30, 28, WHITE);
    }
    // CRONÔMETRO
    int timerRadius = 55;
    int timerX = GetScreenWidth() - timerRadius - 25;
    int timerY = 25 + timerRadius;
    DrawCircle(timerX, timerY, timerRadius, (Color){25,25,30, 220});
    DrawCircleSectorLines((Vector2){timerX, timerY}, timerRadius, 0, 360, 32, (Color){180,180,180,220});
    float angle = (fase2Timer / 30.0f) * 360.0f;
    Color barColor = (fase2Timer <= 10.0f) ? (Color){230,30,30,150} : (Color){80, 230, 80, 130};
    DrawCircleSector((Vector2){timerX, timerY}, timerRadius-4, -90, -90+angle, 60, barColor);
    char timerText[8];
    int tempo = (int)fase2Timer;
    snprintf(timerText, sizeof(timerText), "%02d", tempo);
    int fonte = 49;
    Color timeColor = (fase2Timer <= 5.0f) ? RED : ((fase2Timer <= 10.0f) ? (Color){230,30,30,150} : WHITE);
    if (fase2Timer <= 5.0f) { fonte = 58; }
    int timerTextWidth = MeasureText(timerText, fonte);
    DrawText(timerText, timerX - timerTextWidth/2, timerY - fonte/2, fonte, timeColor);
    DrawText("s", timerX + timerTextWidth/2 + 2, timerY - fonte/3 + 10, 28, (Color){170,170,210,210});
    // RETÂNGULOS OPÇÕES LADO DIREITO
    int numRects = 5;
    int baseWidth = 600;
    int larguraStep = 85;
    int rectHeight = 78;
    int spacing = 28;
    int offsetY = 295;
    int offsetX = 35;
    Color rectGray      = (Color){56, 56, 56, 216};
    Color hoverGreen    = (Color){ 26, 110, 51, 235 };
    Color mouseGreen    = (Color){14, 77, 33, 235};
    Color disabledGray  = (Color){90, 90, 90, 175 };
    Color timeoutRed    = (Color){230, 30, 30, 210};
    Vector2 mouse = GetMousePosition();
    // NOVO ARRAY DE OPÇÕES (com letra e descrição)
    const char* opcoes[5] = {
        "A) Definir IP local da vítima.",
        "B) Bloquear conexões no firewall.",
        "C) Enviar comandos diretamente ao shell do sistema.",
        "D) Executar o shell remoto com privilégios elevados.",
        "E) Criar um socket de comunicação para permitir a conexão."
    };
    for (int i = 0; i < numRects; i++)
    {
        int rectWidth = baseWidth + i * larguraStep;
        int y = offsetY + i * (rectHeight + spacing);
        int x = GetScreenWidth() - rectWidth - offsetX;
        Rectangle rec = {x, y, rectWidth, rectHeight};
        Color cor = rectGray;
        Color txtColor = WHITE;
        if (respostaRespondida) {
            // Já respondeu!
            if (opcaoSelecionadaUsuario == 4) {
                // Acertou (clicou na E), faz E piscar verde
                if (i == 4 && ((int)(blinkTimer * 5.0f) % 2 == 0)) {
                    cor = (Color){26, 110, 51, 255}; // verde forte
                }
            } else {
                // Errou (não clicou na E)
                if (i == opcaoSelecionadaUsuario && ((int)(blinkTimer * 5.0f) % 2 == 0)) {
                    cor = (Color){230, 30, 30, 210}; // vermelho forte
                }
                if (i == 4 && ((int)(blinkTimer * 5.0f) % 2 == 0)) {
                    cor = (Color){26, 110, 51, 255}; // verde forte na opção E
                }
            }
            if(optionDisabled[i]) {
                txtColor = (Color){180,180,180,210};
            }
        } else {
            // Ninguém respondeu ainda: use as cores normais
            if (optionDisabled[i]) {
                if(cronometroDesligado)
                    cor = timeoutRed; // desabilitado por timeout: vermelho
                else
                    cor = disabledGray; // desabilitado por outro motivo: cinza
                txtColor = (cronometroDesligado ? (Color){255,80,80,255} : (Color){180,180,180,210});
            } else if (CheckCollisionPointRec(mouse, rec)) {
                if (!optionDisabled[i]) selectedOption = i;
                cor = mouseGreen;
            } else if (selectedOption == i) {
                cor = hoverGreen;
            }
        }
        DrawRectangleRounded(rec, 0.32f, 16, cor);
        DrawText(
            opcoes[i],
            x + OPTION_TEXT_MARGIN_LEFT, // <== ALTERADO PARA ALINHAR À ESQUERDA
            y + rectHeight/2 - 24/2,
            24, txtColor
        );
    }
    // --- SPRITE DO PERSONAGEM VAI AQUI (DEPOIS de tudo, para FICAR NA FRENTE) ---
    Texture2D spr = sprJoao;
    float scale = 0.6f;
    if(strcmp(name, "Mateus") == 0) {
        if(respostaRespondida && opcaoSelecionadaUsuario == 4) {
            spr = sprMateus2; // Se respondeu E, mostra mateu2.png
            scale = 0.8f;
        } else if(respostaRespondida && opcaoSelecionadaUsuario != -1 && opcaoSelecionadaUsuario != 4) {
            // Errou: mostra mateus3.png na mesma escala
            spr = sprMateus3;
            scale = 0.8f;
        } else {
            spr = sprMateus;
            scale = 1.3f;
        }
    }
    else if(strcmp(name, "João") == 0 || name[0]=='\0') {
        spr = sprJoao;
        scale = 0.6f;
    }
    else if(strcmp(name, "Mamede") == 0) {
        spr = sprMamede;
        scale = 1.0f;
    }
    else if(strcmp(name, "Carlos") == 0) {
        spr = sprCarlos;
        scale = 0.56f;
    }
    float tw2 = spr.width * scale;
    float th = spr.height * scale;
    Vector2 pos;
    pos.x = imgX - 330 + (imgW - tw2)/2.0f;
    pos.y = imgY - th + 210;
    if (strcmp(name, "Carlos") == 0)
        pos.x += 100;
    DrawTextureEx(spr, pos, 0, scale, WHITE); // AGORA REALMENTE SOBRE TUDO
    // === EXIBA O RETÂNGULO E TEXTO DO GEMINI POR ÚLTIMO, SEMPRE NA FRENTE ===
    float geminiX = 49, geminiY = 67, geminiScale = 0.1f;
    float geminiW = sprGemini.width * geminiScale;
    float geminiH = sprGemini.height * geminiScale;
    const char* gemini_msg = !geminiHelpClicked ? gemini_help_msg_default : gemini_help_msg_ajuda;
    int txtSize = 20;
    float rx = geminiX + geminiW - 20.0f;
    float ry = geminiY + (geminiH - geminiRectH)/2.0f;
    float animW = geminiRectW * geminiRectAnim;
    Color logoCol = geminiMouseOver ? (Color){ 18, 60, 32, 255 } : (Color){ 26, 110, 51, 255 };
    if (geminiRectAnim > 0.01f) {
        Color rectCol = (Color){ 15, 42, 26, (unsigned char)( 210 * geminiRectAnim ) };
        float round = 0.33f;
        DrawRectangleRounded((Rectangle){ rx, ry, animW, geminiRectH }, round, 12, rectCol);
        if (geminiRectAnim > 0.05f) {
            if (animW > geminiTextWidth + 2*GEMINI_PAD_X - 4) {
                DrawText(gemini_msg, rx + GEMINI_PAD_X, ry + geminiRectH/2 - txtSize/2, txtSize, WHITE);
            }
        }
    }
    DrawTextureEx(sprGemini, (Vector2){geminiX, geminiY}, 0.0f, geminiScale, logoCol);
    EndDrawing();
}
void UnloadFase2(void)
{
    UnloadTexture(fundo);
    UnloadTexture(pergunta_img);
    UnloadTexture(sprJoao);
    UnloadTexture(sprMateus);
    UnloadTexture(sprMateus2);
    UnloadTexture(sprMateus3);
    UnloadTexture(sprCarlos);
    UnloadTexture(sprMamede);
    UnloadTexture(sprGemini);
}