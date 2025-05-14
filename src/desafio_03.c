#include "desafio_03.h"

typedef struct {
    const char* texto;
    bool correta;
    bool desabilitada;
} DialogueOption;

typedef struct {
    const char* pergunta_txt;
    DialogueOption* opcoes;
    int num_opcoes;
    int opcao_selecionada_usuario;
    bool respondeu;
    float timer_restante;
    float timer_total;
    bool timer_ativo;
    bool timer_explodiu;
} DialogueQuestion;

#define FASE2_FADEOUT_DURACAO 0.8f
#define FALA_NORMAL_03 "Você recebe um e-mail com o assunto: 'Parabéns! Você ganhou um prêmio exclusivo! Clique no link para resgatar agora.'\nO e-mail parece vir de uma empresa conhecida, mas contém erros gramaticais e um link encurtado. O que você faz?"
#define FALA_ACERTO_03  "Escolha certeira! Hank deve ter anotado meu cuidado — estou um passo à frente na vaga."
#define FALA_ERRO_03    "Putz… decisão errada! O sistema de monitoramento disparou e Hank vai notar esse deslize."
#define FALA_JOAO_ACERTO "Ufa, passei por essa sem suar. Venha o próximo desafio!"
#define FALA_JOAO_ERRO   "Droga, cliquei errado! O alarme tocou e o Hank vai questionar minha atenção."
#define FALA_CARLOS_ACERTO "Isso foi tranquilo — vulnerabilidade na cara. Mais pontos pra mim!"
#define FALA_CARLOS_ERRO   "Como deixei passar essa? Um deslize bobo bem na frente do Hank"
#define FALA_MAMEDE_ACERTO "Perfeito. Cada etapa concluída me deixa mais perto da contratação."
#define FALA_MAMEDE_ERRO   "Ah, vacilei justo agora! Preciso compensar nos próximos testes ou perco a chance."
#define GEMINI_RECT_PADRAO 550
#define GEMINI_PAD_X 36

static TypeWriter writer;
static char fala_exibida[512];
static bool podeAvancar = false;
static float respostaShowTimer = 0.0f;
static const float RESPOSTA_MOSTRA_SEG = 5.0f;
static int selectedOption = 0;
static bool geminiHelpClicked = false;
static float geminiRectW = 550;
static float geminiRectH = 0;
static int   geminiTextWidth = 0;
static float geminiRectAnim = 0.0f;
static bool geminiMouseOver = false;
static float geminiAnimSpeed = 6.0f;
static bool fase2_fazendo_fadeout = false;
static float fase2_fadeout_time = 0.0f;
static bool fase_concluida = false; 
static const char* gemini_help_msg_default = "Clique aqui caso precise de ajuda!";
static const char* gemini_help_msg_ajuda = "As opções A, C e D estão erradas";

static Texture2D fundo;
static Texture2D pergunta_img;
static Texture2D sprJoao, sprJoao2, sprJoao3;
static Texture2D sprMateus, sprMateus2, sprMateus3;
static Texture2D sprCarlos, sprCarlos2, sprCarlos3;
static Texture2D sprMamede, sprMamede2, sprMamede3;
static Texture2D sprGemini;
static Texture2D sprEnterButton;

static DialogueQuestion pergunta2;
static DialogueOption opcoesFase2[] = 
{
    {"A) Ignora o e-mail e exclui.", false, false},
    {"B) Encaminha para os amigos para receberem o prémio.", false, false},
    {"C) Clica no link imediatamente para não perder o prêmio.", false, false},
    {"D) Manda seus dados pessoais, afinal, é melhor garantir o prêmio.", false, false},
    {"E) Verifica a origem do e-mail e pesquisa sobre a mensagem antes de agir.", true, false},
};

static void AtualizaTamanhoGeminiBox(void)
{
    float geminiScale = 0.1f;
    float geminiH = sprGemini.height * geminiScale;
    int txtSize = 20;
    const char* gemini_msg = !geminiHelpClicked ? gemini_help_msg_default : gemini_help_msg_ajuda;
    geminiTextWidth = MeasureText(gemini_msg, txtSize);
    float minW = GEMINI_RECT_PADRAO;
    float maxW = GetScreenWidth() - 140;
    geminiRectW = minW;
    if (geminiTextWidth + 2 * GEMINI_PAD_X > minW)
        geminiRectW = (geminiTextWidth + 2 * GEMINI_PAD_X > maxW) ? maxW : geminiTextWidth + 2 * GEMINI_PAD_X;
    geminiRectH = geminiH * 0.75f;
}

const char* FalaPorResultado(const char* name, bool acerto) {
    if (!name || !name[0]) name = "Dante";
    if (strcmp(name, "Alice") == 0)      return acerto ? FALA_JOAO_ACERTO   : FALA_JOAO_ERRO;
    if (strcmp(name, "Jade") == 0)    return acerto ? FALA_CARLOS_ACERTO : FALA_CARLOS_ERRO;
    if (strcmp(name, "Levi") == 0)    return acerto ? FALA_MAMEDE_ACERTO : FALA_MAMEDE_ERRO;
    return acerto ? FALA_ACERTO_03 : FALA_ERRO_03;
}

void InitDialogueQuestion(DialogueQuestion* dq, const char* pergunta_txt, DialogueOption* opcoes, int num_opcoes, float timer_total) {
    if (!dq) return;
    dq->pergunta_txt = pergunta_txt;
    dq->opcoes = opcoes;
    dq->num_opcoes = num_opcoes;
    dq->opcao_selecionada_usuario = -1;
    dq->respondeu = false;
    dq->timer_restante = timer_total;
    dq->timer_total = timer_total;
    dq->timer_ativo = true;
    dq->timer_explodiu = false;
    for (int i=0; i<num_opcoes; ++i)
        dq->opcoes[i].desabilitada = false; // reabilita tudo
}

void UpdateDialogueQuestion(DialogueQuestion* dq, float deltaTime) {
    if (!dq) return;
    if (!dq->timer_explodiu && dq->timer_ativo && !dq->respondeu) {
        dq->timer_restante -= deltaTime;
        if (dq->timer_restante <= 0.0f) {
            dq->timer_restante = 0.0f;
            dq->timer_explodiu = true;
            dq->timer_ativo = false;
            for(int i=0; i<dq->num_opcoes; ++i)
                dq->opcoes[i].desabilitada = true;
        }
    }
}

void DialogueNavigateOptions(const DialogueQuestion* dq, int* selectedIndex, int direction) {
    if (!dq || !selectedIndex) return;
    int idx = *selectedIndex;
    int tries = 0;
    do {
        idx = (idx + direction + dq->num_opcoes) % dq->num_opcoes;
        tries++;
    } while (dq->opcoes[idx].desabilitada && tries < dq->num_opcoes);
    if (!dq->opcoes[idx].desabilitada)
        *selectedIndex = idx;
}

int GetDialogueOptionClick(const DialogueQuestion* dq,
    int offsetX, int offsetY, int baseWidth, int rectHeight, int spacing, int larguraStep)
{
    if (!dq) return -1;
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < dq->num_opcoes; i++) {
        int rectWidth = baseWidth + i * larguraStep;
        int y = offsetY + i * (rectHeight + spacing);
        int x = GetScreenWidth() - rectWidth - offsetX;
        Rectangle rec = {x, y, rectWidth, rectHeight};
        if (!dq->opcoes[i].desabilitada &&
            CheckCollisionPointRec(mouse, rec) &&
            IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            return i;
        }
    }
    return -1;
}

void DrawDialogueOption(const DialogueOption* opt, Rectangle rec, bool selected, bool disabled, bool blink, Color base, Color txtCor) {
    Color cor = base;
    DrawRectangleRounded(rec, 0.32f, 16, cor);
    DrawText(opt->texto, rec.x+36, rec.y+rec.height/2-24/2, 24, txtCor);
}

void DrawAllDialogueOptions(const DialogueQuestion* dq, int selected, int offsetX, int offsetY,
    int baseWidth, int rectHeight, int spacing, int larguraStep,
    int blinkWrong, bool blink, int blinkCorrect)
{
    Color rectGray      = (Color){56, 56, 56, 216};
    Color hoverGreen    = (Color){26,110,51,235};
    Color disabledGray  = (Color){90,90,90,175};
    Vector2 mouse = GetMousePosition();
    for (int i = 0; i < dq->num_opcoes; i++) {
        int rectWidth = baseWidth + i * larguraStep;
        int y = offsetY + i * (rectHeight + spacing);
        int x = GetScreenWidth() - rectWidth - offsetX;
        Rectangle rec = {x, y, rectWidth, rectHeight};
        int mouseOver = CheckCollisionPointRec(mouse, rec);
        bool optSelected = (selected == i);
        Color txtCor = WHITE;
        Color baseCor = rectGray;

        if ((blinkWrong == i) && blink) {
            baseCor = (Color){230, 30, 30, 210};
            txtCor = WHITE;
        }
        else if ((blinkCorrect == i) && blink) {
            baseCor = (Color){26, 110, 51, 255};
            txtCor = WHITE;
        }
        else if (dq->opcoes[i].desabilitada) {
            baseCor = disabledGray;
            txtCor = (Color){180,180,180,210};
        }
        else if (!dq->respondeu && !dq->opcoes[i].desabilitada && (mouseOver || optSelected)) {
            baseCor = hoverGreen;
        }

        DrawDialogueOption(&dq->opcoes[i], rec, optSelected, dq->opcoes[i].desabilitada, blink, baseCor, txtCor);
    }
}

void Init_Desafio_03(void)
{
    fundo      = LoadTexture("src/sprites/empresa3.png");
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
    pergunta_img   = LoadTexture("src/sprites/pergunta3.png");
    sprEnterButton = LoadTexture("src/sprites/enter_button.png");
    
    strcpy(fala_exibida, FALA_NORMAL_03);
    InitDialogueQuestion(&pergunta2, FALA_NORMAL_03, opcoesFase2, 5, 45.0f);
    InitTypeWriter(&writer, fala_exibida, 26.5f);
    AtualizaTamanhoGeminiBox();

    selectedOption        = 0;
    geminiRectAnim        = 0.0f;
    respostaShowTimer     = 0.0f;
    fase2_fadeout_time    = 0.0f;
    geminiHelpClicked     = false;
    geminiMouseOver       = false;
    podeAvancar           = false;
    fase2_fazendo_fadeout = false;
    fase_concluida        = false;
}

void Update_Desafio_03(void)
{
    float delta = GetFrameTime();

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
        if (!geminiHelpClicked && !pergunta2.timer_explodiu && !pergunta2.respondeu) {
            geminiHelpClicked = true;
            AtualizaTamanhoGeminiBox();
            geminiRectAnim = 1.0f;
            pergunta2.opcoes[0].desabilitada = true;
            pergunta2.opcoes[2].desabilitada = true;
            pergunta2.opcoes[3].desabilitada = true;

            if (pergunta2.opcoes[selectedOption].desabilitada) {
                for (int i = 1; i < pergunta2.num_opcoes; i++)
                    if (!pergunta2.opcoes[i].desabilitada) {
                        selectedOption = i;
                        break;
                    }
            }
        }
    }

    UpdateDialogueQuestion(&pergunta2, delta);
    int baseWidth = 600, larguraStep = 85, rectHeight = 78, spacing = 28, offsetY = 295, offsetX = 35;
    Vector2 mouse = GetMousePosition();

    if (!pergunta2.respondeu && pergunta2.timer_explodiu)
    {
        pergunta2.respondeu              = true;
        pergunta2.opcao_selecionada_usuario = 0;
        respostaShowTimer                = 0.0f;
        pergunta2.timer_ativo            = false;

        for (int i = 0; i < pergunta2.num_opcoes; ++i)
            pergunta2.opcoes[i].desabilitada = true;

        const char* name = gSelectedCharacterName;
        strcpy(fala_exibida, FalaPorResultado(name, false));
        InitTypeWriter(&writer, fala_exibida, 26.5f);
        podeAvancar = false;
    }

    if (!pergunta2.respondeu && !pergunta2.timer_explodiu) {
        for (int i = 0; i < pergunta2.num_opcoes; i++) {
            int rectWidth = baseWidth + i * larguraStep;
            int y = offsetY + i * (rectHeight + spacing);
            int x = GetScreenWidth() - rectWidth - offsetX;
            Rectangle rec = {x, y, rectWidth, rectHeight};
            if (!pergunta2.opcoes[i].desabilitada &&
                CheckCollisionPointRec(mouse, rec)) {
                //selectedOption = i;
            }
        }
        if (IsKeyPressed(KEY_DOWN)) DialogueNavigateOptions(&pergunta2, &selectedOption, +1);
        if (IsKeyPressed(KEY_UP))   DialogueNavigateOptions(&pergunta2, &selectedOption, -1);
        int clicada = GetDialogueOptionClick(&pergunta2, 35, 295, 600, 78, 28, 85);
        if (!pergunta2.opcoes[selectedOption].desabilitada &&
            (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)))
            clicada = selectedOption;
        if (clicada != -1) {
            pergunta2.opcao_selecionada_usuario = clicada;
            pergunta2.respondeu = true;
            respostaShowTimer = 0.0f;
            pergunta2.timer_ativo = false;
            for (int i = 0; i < pergunta2.num_opcoes; ++i)
                pergunta2.opcoes[i].desabilitada = true;
            const char* name = gSelectedCharacterName;
            bool acertou = pergunta2.opcoes[clicada].correta;
            strcpy(fala_exibida, FalaPorResultado(name, acertou));
            InitTypeWriter(&writer, fala_exibida, 26.5f);
            podeAvancar = false;
        }
    }
    UpdateTypeWriter(&writer, delta, IsKeyPressed(KEY_SPACE));
    if (!podeAvancar && writer.done) podeAvancar = true;
    if (pergunta2.respondeu) respostaShowTimer += delta;
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

        if (!fase2_fazendo_fadeout) {
            if (CheckCollisionPointRec(mouse, btnBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                fase2_fazendo_fadeout = true;
                fase2_fadeout_time = 0.0f;
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
                fase2_fazendo_fadeout = true;
                fase2_fadeout_time = 0.0f;
            }
        }

        if (fase2_fazendo_fadeout) {
            fase2_fadeout_time += delta;
            if (fase2_fadeout_time >= FASE2_FADEOUT_DURACAO) {
                fase_concluida = true;
            }
        }
    }
}

void Draw_Desafio_03(void)
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
    const char* name = gSelectedCharacterName;
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
    if (!pergunta2.respondeu || respostaShowTimer <= RESPOSTA_MOSTRA_SEG) {
        DrawChronometer(pergunta2.timer_restante, pergunta2.timer_total, GetScreenWidth()-80, 80, 55);
        int baseWidth = 600;
        int larguraStep = 85;
        int rectHeight = 78;
        int spacing = 28;
        int offsetY = 295;
        int offsetX = 35;
        int blinkWrong = -1;
        int blinkCorrect = -1;
        bool blink = false;
        if (pergunta2.respondeu) {
            blink = (((int)(blinkTimer * 5.0f) % 2) == 0);
            if (pergunta2.opcoes[pergunta2.opcao_selecionada_usuario].correta) {
                blinkCorrect = pergunta2.opcao_selecionada_usuario;
            } else {
                blinkWrong = pergunta2.opcao_selecionada_usuario;
                for (int i = 0; i < pergunta2.num_opcoes; ++i)
                    if (pergunta2.opcoes[i].correta) blinkCorrect = i;
            }
        }

        int highlightOption = -1; 
        Vector2 mousePos = GetMousePosition();

        for (int i = 0; i < pergunta2.num_opcoes; i++) {
            if (pergunta2.opcoes[i].desabilitada) continue;

            int rectWidth = baseWidth + i * larguraStep;
            int y  = offsetY + i * (rectHeight + spacing);
            int x  = GetScreenWidth() - rectWidth - offsetX;
            Rectangle rec = {x, y, rectWidth, rectHeight};

            if (CheckCollisionPointRec(mousePos, rec)) {
                highlightOption = i;
                break;
            }
        }
        DrawAllDialogueOptions(&pergunta2,
                       highlightOption,
                       offsetX, offsetY,
                       baseWidth, rectHeight,
                       spacing, larguraStep,
                       blinkWrong, blink, blinkCorrect
        );

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
    if(strcmp(name, "Dante") == 0) {
        if(pergunta2.respondeu && pergunta2.opcoes[pergunta2.opcao_selecionada_usuario].correta) {
            spr = sprMateus2; scale = 0.8f;
        } else if(pergunta2.respondeu && pergunta2.opcao_selecionada_usuario != -1) {
            spr = sprMateus3; scale = 0.8f;
        } else {
            spr = sprMateus; scale = 1.3f;
        }
    } else if(strcmp(name, "Alice") == 0) {
        if(pergunta2.respondeu && pergunta2.opcoes[pergunta2.opcao_selecionada_usuario].correta) {
            spr = sprJoao2; scale = 0.95f;
        } else if(pergunta2.respondeu && pergunta2.opcao_selecionada_usuario != -1) {
            spr = sprJoao3; scale = 0.95f;
        } else {
            spr = sprJoao; scale = 0.6f;
        }
    } else if(strcmp(name, "Jade") == 0) {
        if(pergunta2.respondeu && pergunta2.opcoes[pergunta2.opcao_selecionada_usuario].correta) {
            spr = sprCarlos2; scale = 1.02f; carlosExtraOffset = -70;
        } else if(pergunta2.respondeu && pergunta2.opcao_selecionada_usuario != -1) {
            spr = sprCarlos3; scale = 1.0f; carlosExtraOffset = -44;
        } else {
            spr = sprCarlos; scale = 0.56f; carlosExtraOffset = 0;
        }
    } else if(strcmp(name, "Levi") == 0) {
        if(pergunta2.respondeu && pergunta2.opcoes[pergunta2.opcao_selecionada_usuario].correta) {
            spr = sprMamede2; scale = 1.0f;
        } else if(pergunta2.respondeu && pergunta2.opcao_selecionada_usuario != -1) {
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
    if (strcmp(name, "Jade") == 0)
        pos.x += 100 + carlosExtraOffset;
    else if (strcmp(name, "Levi") == 0)
        pos.x += mamedeExtraOffset;
    DrawTextureEx(spr, pos, 0, scale, WHITE);

    if (pergunta2.respondeu && !pergunta2.opcoes[pergunta2.opcao_selecionada_usuario].correta && pergunta2.opcao_selecionada_usuario != -1) {
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
    if (pergunta2.respondeu && respostaShowTimer > RESPOSTA_MOSTRA_SEG) {
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
    if(fase2_fazendo_fadeout) {
        float perc = fase2_fadeout_time / FASE2_FADEOUT_DURACAO;
        if (perc > 1.0f) perc = 1.0f;
        int alpha = (int)(255 * perc);
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), (Color){0,0,0, alpha});
    }
    EndDrawing();
}

bool Fase_Desafio_03_Concluida(void)
{
    return fase_concluida;
}

void Unload_Desafio_03(void)
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