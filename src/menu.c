#include "menu.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#define FRAME_COUNT 15
#define MAX_COLS 64

typedef struct Node
{
    char nome[32];
    struct Node *prox, *ant;
} Node;

typedef enum
{
    MENU_STATE_MAIN,
    MENU_STATE_CHOOSE_CHAR,
    MENU_STATE_DONE
} MenuState;

static MenuState currentMenuState = MENU_STATE_MAIN;

// Texturas e efeitos
static Texture2D matrixSprite;
static Texture2D jogoTexture;
static Rectangle matrixFrames[FRAME_COUNT];
static int currentFrame = 0;
static float frameDuration = 0.05f;
static float lastFrameTime = 0.0f;
static float spriteY[MAX_COLS] = {0};
static bool matrixInited = false;

// Lista circular fixa de personagens
static Node *head = NULL;
static Node *tail = NULL;
static Node *selecionado = NULL;
static int numChars = 0;
static char nomeEscolhido[32] = "";

// Som
static Sound buttonSound;
static Rectangle lastHovered = {0};
static bool hoveredLastFrame = false;

// --- Funções auxiliares ---

void PlayButtonSoundOnce(Rectangle btn, bool isHoveredNow)
{
    if (isHoveredNow && (!hoveredLastFrame || memcmp(&btn, &lastHovered, sizeof(Rectangle)) != 0))
    {
        PlaySound(buttonSound);
        lastHovered = btn;
    }
    hoveredLastFrame = isHoveredNow;
}

// --- Lista circular ---

void CriarListaPersonagens(void)
{
    if (head)
        return;
    Node *n1 = malloc(sizeof(Node));
    Node *n2 = malloc(sizeof(Node));
    Node *n3 = malloc(sizeof(Node));
    Node *n4 = malloc(sizeof(Node));
    strcpy(n1->nome, "João");
    strcpy(n2->nome, "Mateus");
    strcpy(n3->nome, "Carlos");
    strcpy(n4->nome, "Mamede");

    n1->prox = n2;
    n2->prox = n3;
    n3->prox = n4;
    n4->prox = n1;
    n1->ant = n4;
    n2->ant = n1;
    n3->ant = n2;
    n4->ant = n3;

    head = n1;
    tail = n4;
    selecionado = head;
    numChars = 4;
}

void DestruirListaPersonagens(void)
{
    if (head)
    {
        Node *p = head->prox;
        while (p != head)
        {
            Node *aux = p;
            p = p->prox;
            free(aux);
        }
        free(head);
        head = tail = selecionado = NULL;
        numChars = 0;
    }
}

// --- Menu ---
void InitMenu(void)
{
    DestruirListaPersonagens();
    currentMenuState = MENU_STATE_MAIN;
    matrixSprite = LoadTexture("src/sprites/Matrix.png");
    jogoTexture = LoadTexture("src/sprites/jogo.png");
    buttonSound = LoadSound("src/music/buttonPress.wav");

    Rectangle frames[FRAME_COUNT] = {
        {0, 512, 512, 512}, {0, 1024, 512, 512}, {0, 1536, 512, 512}, {512, 0, 512, 512}, {512, 512, 512, 512}, {512, 1024, 512, 512}, {512, 1536, 512, 512}, {1024, 0, 512, 512}, {1024, 512, 512, 512}, {1024, 1024, 512, 512}, {1024, 1536, 512, 512}, {1536, 0, 512, 512}, {1536, 512, 512, 512}, {1536, 1024, 512, 512}, {1536, 1536, 512, 512}};
    for (int i = 0; i < FRAME_COUNT; i++)
        matrixFrames[i] = frames[i];

    currentFrame = 0;
    lastFrameTime = 0.0f;
    matrixInited = false;
    hoveredLastFrame = false;
    CriarListaPersonagens();
    nomeEscolhido[0] = '\0';
}

void UpdateMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float now = GetTime();

    if (now - lastFrameTime >= frameDuration)
    {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrameTime = now;
    }

    Vector2 mouse = GetMousePosition();

    if (currentMenuState == MENU_STATE_MAIN)
    {
        Rectangle btnRect = {w / 2 - 150, h - 180, 300, 80};
        bool hovered = CheckCollisionPointRec(mouse, btnRect);
        PlayButtonSoundOnce(btnRect, hovered);

        if ((hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
        {
            PlaySound(buttonSound);
            currentMenuState = MENU_STATE_CHOOSE_CHAR;
        }
    }

    else if (currentMenuState == MENU_STATE_CHOOSE_CHAR)
    {
        int btnW = 160, btnH = 80, spaceBetween = 20;
        int btnTotalW = numChars * btnW + (numChars - 1) * spaceBetween;
        int btnStartX = (w - btnTotalW) / 2;
        int btnY = h / 2 - btnH / 2;

        Node *p = head;
        for (int i = 0; i < numChars; i++, p = p->prox)
        {
            Rectangle btnRect = {btnStartX + i * (btnW + spaceBetween), btnY, btnW, btnH};
            bool hovered = CheckCollisionPointRec(mouse, btnRect);
            PlayButtonSoundOnce(btnRect, hovered);

            if (hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                PlaySound(buttonSound);
                selecionado = p;
            }
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            PlaySound(buttonSound);
            selecionado = selecionado->prox;
        }
        else if (IsKeyPressed(KEY_LEFT))
        {
            PlaySound(buttonSound);
            selecionado = selecionado->ant;
        }

        Rectangle confirmBtn = {w / 2 - 130, btnY + btnH + 50, 260, 56};
        bool hoveredConfirm = CheckCollisionPointRec(mouse, confirmBtn);
        PlayButtonSoundOnce(confirmBtn, hoveredConfirm);

        if ((hoveredConfirm && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER))
        {
            PlaySound(buttonSound);
            currentMenuState = MENU_STATE_DONE;
        }
    }
}

void DrawMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    BeginDrawing();
    ClearBackground(BLACK);

    float scale = 0.3f;
    int spriteW = 512 * scale;
    int spriteH = 512 * scale;
    int numCols = w / spriteW + 2;

    if (!matrixInited)
    {
        for (int i = 0; i < MAX_COLS; i++)
            spriteY[i] = GetRandomValue(-spriteH, h);
        matrixInited = true;
    }

    float speed = 100.0f * GetFrameTime();
    Color semiTransparentGreen = (Color){0, 255, 0, 60};

    for (int i = 0; i < numCols && i < MAX_COLS; i++)
    {
        spriteY[i] += speed;
        if (spriteY[i] > h)
            spriteY[i] = -spriteH;
        float x = i * spriteW;
        float y = spriteY[i];
        DrawTexturePro(matrixSprite, matrixFrames[currentFrame], (Rectangle){x, y, spriteW, spriteH}, (Vector2){0, 0}, 0.0f, semiTransparentGreen);
    }

    if (currentMenuState == MENU_STATE_MAIN)
    {
        float imgScale = 0.2f;
        float imgWidth = jogoTexture.width * imgScale;
        float imgHeight = jogoTexture.height * imgScale;
        float posX = (w - imgWidth) / 2;
        float posY = (h - imgHeight) / 2;
        DrawTextureEx(jogoTexture, (Vector2){posX, posY}, 0.0f, imgScale, WHITE);

        Rectangle btnRect = {w / 2 - 150, h - 180, 300, 80};
        Color btnColor = CheckCollisionPointRec(GetMousePosition(), btnRect) ? (Color){0, 200, 0, 255} : (Color){0, 180, 0, 255};
        DrawRectangleRec(btnRect, btnColor);
        DrawText("Iniciar jogo", btnRect.x + 50, btnRect.y + 20, 36, WHITE);
    }

    else if (currentMenuState == MENU_STATE_CHOOSE_CHAR)
    {
        int spaceBetween = 20, btnW = 160, btnH = 80;
        int btnTotalW = numChars * btnW + (numChars - 1) * spaceBetween;
        int btnStartX = (w - btnTotalW) / 2;
        int btnY = h / 2 - btnH / 2;
        int fontSize = 36;

        Node *p = head;
        for (int i = 0; i < numChars; i++, p = p->prox)
        {
            Rectangle btnRect = {btnStartX + i * (btnW + spaceBetween), btnY, btnW, btnH};
            Color btnColor = (p == selecionado) ? (Color){40, 100, 40, 255} : BLACK;
            if (CheckCollisionPointRec(GetMousePosition(), btnRect))
            {
                btnColor = (p == selecionado) ? (Color){60, 160, 60, 255} : (Color){60, 60, 60, 255};
            }
            DrawRectangleRec(btnRect, btnColor);
            int textW = MeasureText(p->nome, fontSize);
            DrawText(p->nome, btnRect.x + (btnW - textW) / 2, btnRect.y + (btnH - fontSize) / 2, fontSize, WHITE);
            DrawRectangleLinesEx(btnRect, 3, (p == selecionado) ? RAYWHITE : DARKGRAY);
        }

        DrawText("Escolha seu personagem", w / 2 - 180, btnY - 70, 28, RAYWHITE);
        const char *instrText = "Clique em um personagem, use < > ou pressione ENTER";
        int instrFont = 24;
        int instrTextW = MeasureText(instrText, instrFont);
        DrawText(instrText, (w - instrTextW) / 2, btnY + btnH + 200, instrFont, GRAY);

        Rectangle confirmBtn = {w / 2 - 130, btnY + btnH + 50, 260, 56};
        Color confirmColor = CheckCollisionPointRec(GetMousePosition(), confirmBtn) ? (Color){0, 200, 0, 255} : (Color){20, 80, 20, 255};
        DrawRectangleRec(confirmBtn, confirmColor);
        const char *confText = "Confirmar";
        int confFont = 32;
        int confTw = MeasureText(confText, confFont);
        DrawText(confText, confirmBtn.x + (260 - confTw) / 2, confirmBtn.y + (56 - confFont) / 2, confFont, WHITE);
    }

    EndDrawing();
}

bool MenuStartGame(void)
{
    return (currentMenuState == MENU_STATE_DONE);
}

int MenuSelectedCharacter(void)
{
    Node *p = head;
    int idx = 0;
    while (p && p != selecionado)
    {
        p = p->prox;
        idx++;
        if (p == head)
            break;
    }
    return idx;
}

const char *MenuSelectedCharacterName(void)
{
    return nomeEscolhido;
}

void UnloadMenu(void)
{
    if (selecionado)
        strncpy(nomeEscolhido, selecionado->nome, sizeof(nomeEscolhido));
    else
        nomeEscolhido[0] = '\0';

    UnloadSound(buttonSound);
    UnloadTexture(jogoTexture);
    UnloadTexture(matrixSprite);
    DestruirListaPersonagens();
}
