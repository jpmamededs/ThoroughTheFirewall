#include "fase5.h"
#include "generalFunctions.h"
#include "menu.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
static Texture2D fundo;
static Texture2D pergunta_img;
static Texture2D sprJoao, sprJoao2, sprJoao3;
static Texture2D sprMateus, sprMateus2, sprMateus3;
static Texture2D sprCarlos, sprCarlos2, sprCarlos3;
static Texture2D sprMamede, sprMamede2, sprMamede3;
static Texture2D sprGemini;
static Texture2D sprEnterButton;
static bool fase5_fazendo_fadeout = false;
static float fase5_fadeout_time = 0.0f;
#define FASE5_FADEOUT_DURACAO 0.8f

static DialogueOption opcoesFase5[] = {
    
    {"A) Só consigo acesso físico à máquina.", false, false},
    {"B) Envio um link falso e aguardo um clique.", false, false},
    {"C) Configuro um servidor VPN para a vítima acessar.", false, false},
    {"D) Preciso escanear portas até achar uma aberta e conectar.", false, false},
    {"E) O alvo abre uma conexão para meu endereço, e assim recebo o shell.", true, false}, // correta
};
static DialogueQuestion pergunta5;
#define FALA5_NORMAL \
    "Acho que estou deixando escapar alguma coisa...\n" \
    "Se eu quiser ganhar acesso, como a etapa de conexão deveria acontecer?"
#define FALA5_ACERTO        "Naturalmente! Só poderia ser isso, afinal, quem mais entenderia tão bem de reverse shell quanto eu?"
#define FALA5_ERRO          "Droga, isso não está certo! O alarme está tocando alto, já sabem onde estou!"
#define FALA5_JOAO_ACERTO   "Hah! Nada escapa do meu radar. Fico à espreita e deixo o alvo vir até mim, como sempre faço."
#define FALA5_JOAO_ERRO     "Assim não... O alarme disparou, vão me encontrar se eu não corrigir logo."
#define FALA5_CARLOS_ACERTO "Óbvio! Só mesmo alguém do meu calibre para receber o shell pela conexão da vítima sem esforço."
#define FALA5_CARLOS_ERRO   "Eita... errei feio! Esse barulho é o alarme avisando todo mundo da minha tentativa!"
#define FALA5_MAMEDE_ACERTO "É claro, lembrei rapidinho! Só preciso aguardar de braços cruzados a vítima pedir minha atenção."
#define FALA5_MAMEDE_ERRO   "Que mancada! O caminho estava errado e agora esse alarme maldito vai mostrar pra todos minha tentativa!"

static TypeWriter writer;
static char fala_exibida[512];
static bool podeAvancar = false;
static float respostaShowTimer = 0.0f;
static const float RESPOSTA_MOSTRA_SEG = 5.0f;
static int selectedOption = 0;
static bool geminiHelpClicked = false;
static const char* gemini_help_msg_default = "Clique aqui caso precise de ajuda!";
static const char* gemini_help_msg_ajuda   = "Em uma reverse shell típica, a vítima que conecta ao servidor do atacante.";
static float geminiRectW = 550;
static float geminiRectH = 0;
static int   geminiTextWidth = 0;
static float geminiRectAnim = 0.0f;
static bool geminiMouseOver = false;
static float geminiAnimSpeed = 6.0f;
#define GEMINI_RECT_PADRAO 550
#define GEMINI_PAD_X 36

void AtualizaTamanhoGeminiBox5(void) {
    float geminiScale = 0.1f;
    float geminiH = sprGemini.height * geminiScale;
    int txtSize = 20;
    const char* gemini_msg = !geminiHelpClicked ? gemini_help_msg_default : gemini_help_msg_ajuda;
    geminiTextWidth = MeasureText(gemini_msg, txtSize);
    float minW = GEMINI_RECT_PADRAO;
    float maxW = GetScreenWidth() - 140;
    geminiRectW = minW;
    if (geminiTextWidth + 2*GEMINI_PAD_X > minW)
        geminiRectW = (geminiTextWidth + 2*GEMINI_PAD_X > maxW) ? maxW : geminiTextWidth + 2*GEMINI_PAD_X;
    geminiRectH = geminiH * 0.75f;
}

extern AppState state;

const char* FalaPorResultado5(const char* name, bool acerto) {
    if (!name || !name[0]) name = "Mateus";
    if (strcmp(name, "João") == 0)      return acerto ? FALA5_JOAO_ACERTO   : FALA5_JOAO_ERRO;
    if (strcmp(name, "Carlos") == 0)    return acerto ? FALA5_CARLOS_ACERTO : FALA5_CARLOS_ERRO;
    if (strcmp(name, "Mamede") == 0)    return acerto ? FALA5_MAMEDE_ACERTO : FALA5_MAMEDE_ERRO;
    return acerto ? FALA5_ACERTO : FALA5_ERRO;
}

void InitFase5(void)
{
    fundo = LoadTexture("src/sprites/empresa4.jpg");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    sprJoao    = LoadTexture("src/sprites/joaoSprite.png");
    sprJoao2   = LoadTexture("src/sprites/joao2.png");
    sprJoao3   = LoadTexture("src/sprites/joao3.png");
    sprMateus  = LoadTexture("src/sprites/mateusSprite.png");
    sprMateus2 = LoadTexture("src/sprites/mateus2.png");
    sprMateus3 = LoadTexture("src/sprites/mateus3.png");
    sprCarlos  = LoadTexture("src/sprites/carlosSprite.png");
    sprCarlos2 = LoadTexture("src/sprites/carlos2.png");
    sprCarlos3 = LoadTexture("src/sprites/carlos3.png");
    sprMamede  = LoadTexture("src/sprites/mamedeSprite.png");
    sprMamede2 = LoadTexture("src/sprites/mamede2.png");
    sprMamede3 = LoadTexture("src/sprites/mamede3.png");
    sprGemini  = LoadTexture("src/sprites/os/gemini.png");
    sprEnterButton  = LoadTexture("src/sprites/enter_button.png");

    InitDialogueQuestion(&pergunta5, FALA5_NORMAL, opcoesFase5, 5, 30.0f);
    strcpy(fala_exibida, FALA5_NORMAL);
    InitTypeWriter(&writer, fala_exibida, 18.5f);
    selectedOption = 0;
    geminiHelpClicked = false;
    geminiRectAnim = 0.0f;
    geminiMouseOver = false;
    podeAvancar = false;
    respostaShowTimer = 0.0f;
    AtualizaTamanhoGeminiBox5();
    fase5_fazendo_fadeout = false;
    fase5_fadeout_time = 0.0f;
}

void UpdateFase5(void)
{
    float delta = GetFrameTime();
    float geminiX = 49, geminiY = 67, geminiScale = 0.1f;
    float geminiW = sprGemini.width * geminiScale;
    float geminiH = sprGemini.height * geminiScale;
    AtualizaTamanhoGeminiBox5();
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
        if (!geminiHelpClicked && !pergunta5.timer_explodiu && !pergunta5.respondeu) {
            geminiHelpClicked = true;
            AtualizaTamanhoGeminiBox5();
            geminiRectAnim = 1.0f;
            // Dica elimina só a D!
            pergunta5.opcoes[3].desabilitada = true; // D
            if (pergunta5.opcoes[selectedOption].desabilitada) {
                for (int i = 0; i < pergunta5.num_opcoes; i++)
                    if (!pergunta5.opcoes[i].desabilitada) {
                        selectedOption = i;
                        break;
                    }
            }
        }
    }

    UpdateDialogueQuestion(&pergunta5, delta);
    int baseWidth = 600, larguraStep = 85, rectHeight = 78, spacing = 28, offsetY = 295, offsetX = 35;
    Vector2 mouse = GetMousePosition();
    if (!pergunta5.respondeu && !pergunta5.timer_explodiu) {
        for (int i = 0; i < pergunta5.num_opcoes; i++) {
            int rectWidth = baseWidth + i * larguraStep;
            int y = offsetY + i * (rectHeight + spacing);
            int x = GetScreenWidth() - rectWidth - offsetX;
            Rectangle rec = {x, y, rectWidth, rectHeight};
            if (!pergunta5.opcoes[i].desabilitada &&
                CheckCollisionPointRec(mouse, rec)) {
                selectedOption = i;
            }
        }
        if (IsKeyPressed(KEY_DOWN)) DialogueNavigateOptions(&pergunta5, &selectedOption, +1);
        if (IsKeyPressed(KEY_UP))   DialogueNavigateOptions(&pergunta5, &selectedOption, -1);
        int clicada = GetDialogueOptionClick(&pergunta5, 35, 295, 600, 78, 28, 85);
        if (!pergunta5.opcoes[selectedOption].desabilitada &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)))
            clicada = selectedOption;
        if (clicada != -1) {
            pergunta5.opcao_selecionada_usuario = clicada;
            pergunta5.respondeu = true;
            respostaShowTimer = 0.0f;
            pergunta5.timer_ativo = false;
            for (int i = 0; i < pergunta5.num_opcoes; ++i)
                pergunta5.opcoes[i].desabilitada = true;
            const char* name = MenuSelectedCharacterName();
            bool acertou = pergunta5.opcoes[clicada].correta;
            strcpy(fala_exibida, FalaPorResultado5(name, acertou));
            InitTypeWriter(&writer, fala_exibida, 18.5f);
            podeAvancar = false;
        }
    }
    UpdateTypeWriter(&writer, delta, IsKeyPressed(KEY_SPACE));
    if (!podeAvancar && writer.done) podeAvancar = true;
    if (pergunta5.respondeu) respostaShowTimer += delta;
    if (podeAvancar && (respostaShowTimer > RESPOSTA_MOSTRA_SEG)) {
        float pulse = 0.07f * sinf(GetTime() * 3.0f);
        float btnScaleBase = 0.85f;
        float btnScale = btnScaleBase + pulse;
        float btnW = sprEnterButton.width * btnScale;
        float btnH = sprEnterButton.height * btnScale;
        float btnX = GetScreenWidth()/2 - btnW/2 + 120;
        float btnY = GetScreenHeight()/2 - 150;
        Rectangle btnBounds = {btnX, btnY, btnW, btnH};
        Vector2 mouse = GetMousePosition();
        if (!fase5_fazendo_fadeout) {
            if (CheckCollisionPointRec(mouse, btnBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                fase5_fazendo_fadeout = true;
                fase5_fadeout_time = 0.0f;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                fase5_fazendo_fadeout = true;
                fase5_fadeout_time = 0.0f;
            }
        }
        if (fase5_fazendo_fadeout) {
            fase5_fadeout_time += delta;
            if (fase5_fadeout_time >= FASE5_FADEOUT_DURACAO) {
                state = APP_PC_SCREEN;
            }
        }
    }
}

void DrawFase5(void)
{
    static float blinkTimer = 0.0f;
    float delta = GetFrameTime();
    blinkTimer += delta;

    BeginDrawing();
    ClearBackground(RAYWHITE);
    DrawTextureEx(fundo, (Vector2){0,0}, 0.0f, (float)GetScreenWidth() / fundo.width, RAYWHITE);
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

    const char* name = MenuSelectedCharacterName();
    DrawText((name && name[0]) ? name : "???", imgX + 10, imgY + imgH - 30, 30, WHITE);

    int borderRadius = boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});

    if (writer.drawnChars > 0) {
        char txtbuf[512];
        strncpy(txtbuf, fala_exibida, writer.drawnChars);
        txtbuf[writer.drawnChars] = '\0';
        DrawText(txtbuf, boxX + 20, boxY + 30, 28, WHITE);
    }

    if (!pergunta5.respondeu || respostaShowTimer <= RESPOSTA_MOSTRA_SEG) {
        DrawChronometer(pergunta5.timer_restante, pergunta5.timer_total, GetScreenWidth()-80, 80, 55);

        int baseWidth = 600;
        int larguraStep = 85;
        int rectHeight = 78;
        int spacing = 28;
        int offsetY = 295;
        int offsetX = 35;

        int blinkWrong = -1;
        int blinkCorrect = -1;
        bool blink = false;
        if (pergunta5.respondeu) {
            blink = (((int)(blinkTimer * 5.0f) % 2) == 0);
            if (pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta) {
                blinkCorrect = pergunta5.opcao_selecionada_usuario;
            } else {
                blinkWrong = pergunta5.opcao_selecionada_usuario;
                for (int i = 0; i < pergunta5.num_opcoes; ++i)
                    if (pergunta5.opcoes[i].correta) blinkCorrect = i;
            }
        }
        DrawAllDialogueOptions(&pergunta5, selectedOption, offsetX, offsetY, baseWidth, rectHeight, spacing, larguraStep,
                               blinkWrong, blink, blinkCorrect);

        // Gemini
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
    }

    Texture2D spr = sprJoao;
    float scale = 0.6f;
    int carlosExtraOffset = 0, mamedeExtraOffset = 0;
    if(strcmp(name, "Mateus") == 0) {
        if(pergunta5.respondeu && pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta) {
            spr = sprMateus2; scale = 0.8f;
        } else if(pergunta5.respondeu && !pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta && pergunta5.opcao_selecionada_usuario != -1) {
            spr = sprMateus3; scale = 0.8f;
        } else {
            spr = sprMateus; scale = 1.3f;
        }
    } else if(strcmp(name, "João") == 0) {
        if(pergunta5.respondeu && pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta) {
            spr = sprJoao2; scale = 0.95f;
        } else if(pergunta5.respondeu && !pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta && pergunta5.opcao_selecionada_usuario != -1) {
            spr = sprJoao3; scale = 0.95f;
        } else {
            spr = sprJoao; scale = 0.6f;
        }
    } else if(strcmp(name, "Carlos") == 0) {
        if(pergunta5.respondeu && pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta) {
            spr = sprCarlos2; scale = 1.02f; carlosExtraOffset = -70;
        } else if(pergunta5.respondeu && !pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta && pergunta5.opcao_selecionada_usuario != -1) {
            spr = sprCarlos3; scale = 1.0f; carlosExtraOffset = -44;
        } else {
            spr = sprCarlos; scale = 0.56f; carlosExtraOffset = 0;
        }
    } else if(strcmp(name, "Mamede") == 0) {
        if(pergunta5.respondeu && pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta) {
            spr = sprMamede2; scale = 1.0f;
        } else if(pergunta5.respondeu && !pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta && pergunta5.opcao_selecionada_usuario != -1) {
            spr = sprMamede3; scale = 1.0f;
        } else {
            spr = sprMamede; scale = 1.0f;
        }
    } else if(name[0]=='\0') {
        spr = sprJoao; scale = 0.6f;
    }
    float tw2 = spr.width * scale;
    float th  = spr.height * scale;
    Vector2 pos;
    pos.x = imgX - 330 + (imgW - tw2)/2.0f;
    pos.y = imgY - th + 210;
    if (strcmp(name, "Carlos") == 0)
        pos.x += 100 + carlosExtraOffset;
    else if (strcmp(name, "Mamede") == 0)
        pos.x += mamedeExtraOffset;
    DrawTextureEx(spr, pos, 0, scale, WHITE);

    if (pergunta5.respondeu && !pergunta5.opcoes[pergunta5.opcao_selecionada_usuario].correta && pergunta5.opcao_selecionada_usuario != -1) {
        int w = GetScreenWidth(), h = GetScreenHeight();
        int layers = 5, layerThick = 20;
        for (int i = 0; i < layers; i++) {
            int thick = layerThick + i * layerThick;
            int alpha = 38 - i*5;
            if (alpha < 6) alpha = 6;
            Color blurRed = (Color){255, 32, 32, alpha};
            DrawRectangle(0, 0, w, thick, blurRed);
            DrawRectangle(0, h-thick, w, thick, blurRed);
            DrawRectangle(0, 0, thick, h, blurRed);
            DrawRectangle(w-thick, 0, thick, h, blurRed);
        }
    }

    if (pergunta5.respondeu && respostaShowTimer > RESPOSTA_MOSTRA_SEG) {
        float pulse = 0.07f * sinf(GetTime() * 5.0f);
        float btnScaleBase = 0.95f;
        float btnScale = btnScaleBase + pulse;
        float btnW = sprEnterButton.width * btnScale;
        float btnH = sprEnterButton.height * btnScale;
        float btnX = GetScreenWidth()/2 - btnW/2 + 120;
        float btnY = GetScreenHeight()/2 - 150;
        Color sombra = (Color){0, 0, 0, 90};
        DrawRectangleRounded((Rectangle){btnX + 8, btnY + 8, btnW, btnH}, 0.25f, 10, sombra);
        Color brilho = (pulse > 0.04f) ? (Color){255,255,255,75} : WHITE;
        DrawTextureEx(sprEnterButton, (Vector2){btnX, btnY}, 0.0f, btnScale, brilho);
    }
    if(fase5_fazendo_fadeout) {
        float perc = fase5_fadeout_time / FASE5_FADEOUT_DURACAO;
        if (perc > 1.0f) perc = 1.0f;
        int alpha = (int)(255 * perc);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0,0,0, alpha});
    }
    EndDrawing();
}

void UnloadFase5(void)
{
    UnloadTexture(fundo);
    UnloadTexture(pergunta_img);
    UnloadTexture(sprJoao);    UnloadTexture(sprJoao2);    UnloadTexture(sprJoao3);
    UnloadTexture(sprMateus);  UnloadTexture(sprMateus2);  UnloadTexture(sprMateus3);
    UnloadTexture(sprCarlos);  UnloadTexture(sprCarlos2);  UnloadTexture(sprCarlos3);
    UnloadTexture(sprMamede);  UnloadTexture(sprMamede2);  UnloadTexture(sprMamede3);
    UnloadTexture(sprGemini);
    UnloadTexture(sprEnterButton);
}