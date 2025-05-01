#include "menu.h"
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#define FRAME_COUNT 15
#define MAX_COLS 64

typedef struct Node {
    char nome[32];
    struct Node *prox, *ant;
} Node;

typedef enum {
    MENU_STATE_MAIN,
    MENU_STATE_CHOOSE_CHAR,
    MENU_STATE_DONE
} MenuState;

static MenuState currentMenuState = MENU_STATE_MAIN;
// Matrix effect stuff
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
// Salva o nome do personagem escolhido mesmo após destruir a lista
static char nomeEscolhido[32] = "";

void CriarListaPersonagens(void) {
    if (head) return; // Já criado
    Node *n1 = malloc(sizeof(Node));
    Node *n2 = malloc(sizeof(Node));
    Node *n3 = malloc(sizeof(Node));
    Node *n4 = malloc(sizeof(Node));
    strcpy(n1->nome, "João");
    strcpy(n2->nome, "Mateus");
    strcpy(n3->nome, "Carlos");
    strcpy(n4->nome, "Mamede");

    n1->prox = n2;  n2->prox = n3;  n3->prox = n4;  n4->prox = n1;
    n1->ant  = n4;  n2->ant  = n1;  n3->ant  = n2;  n4->ant  = n3;

    head = n1;
    tail = n4;
    selecionado = head;
    numChars = 4;
}

void DestruirListaPersonagens(void) {
    if (head) {
        Node *p = head->prox;
        while (p != head) {
            Node *aux = p;
            p = p->prox;
            free(aux);
        }
        free(head);
        head = tail = selecionado = NULL;
        numChars = 0;
    }
}

void InitMenu(void)
{
    DestruirListaPersonagens(); // Garante não dar leak se rodar mais de uma vez
    currentMenuState = MENU_STATE_MAIN;
    matrixSprite = LoadTexture("src/sprites/Matrix.png");
    jogoTexture = LoadTexture("src/sprites/jogo.png");
    Rectangle frames[FRAME_COUNT] = {
        {0, 512, 512, 512}, {0, 1024, 512, 512}, {0, 1536, 512, 512},
        {512, 0, 512, 512}, {512, 512, 512, 512}, {512, 1024, 512, 512},
        {512, 1536, 512, 512}, {1024, 0, 512, 512}, {1024, 512, 512, 512},
        {1024, 1024, 512, 512}, {1024, 1536, 512, 512}, {1536, 0, 512, 512},
        {1536, 512, 512, 512}, {1536, 1024, 512, 512}, {1536, 1536, 512, 512}
    };
    for (int i = 0; i < FRAME_COUNT; i++) matrixFrames[i] = frames[i];
    currentFrame = 0;
    frameDuration = 0.05f;
    lastFrameTime = 0.0f;
    matrixInited = false;
    CriarListaPersonagens();
    nomeEscolhido[0] = '\0'; // limpa o nome salvo ao reiniciar o menu
}

void UpdateMenu(void)
{
    int w = GetScreenWidth();
    int h = GetScreenHeight();
    float now = GetTime();
    if (now - lastFrameTime >= frameDuration) {
        currentFrame = (currentFrame + 1) % FRAME_COUNT;
        lastFrameTime = now;
    }
    if (currentMenuState == MENU_STATE_MAIN) {
        int btnW = 300;
        int btnH = 80;
        int btnX = w / 2 - btnW / 2;
        int btnY = h - btnH - 100;
        Rectangle btnRect = {btnX, btnY, btnW, btnH};
        Vector2 mouse = GetMousePosition();
        bool hovered = CheckCollisionPointRec(mouse, btnRect);
        if ((hovered && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) || IsKeyPressed(KEY_ENTER)) {
            currentMenuState = MENU_STATE_CHOOSE_CHAR;
        }
    }
    else if (currentMenuState == MENU_STATE_CHOOSE_CHAR) {
        int spaceBetween = 20; // espaço entre os botões
        int btnW = 160;
        int btnH = 80;
        int btnTotalW = numChars * btnW + (numChars - 1) * spaceBetween;
        int btnStartX = (w - btnTotalW) / 2;
        int btnY = h / 2 - btnH / 2;
        Vector2 mouse = GetMousePosition();

        // Botões de personagem
        Node *p = head;
        for (int i = 0; i < numChars; i++, p = p->prox) {
            Rectangle btnRect = {btnStartX + i*(btnW + spaceBetween), btnY, btnW, btnH};
            if (CheckCollisionPointRec(mouse, btnRect)) {
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
                    selecionado = p;
                }
            }
        }

        // Navegação circular por ponteiros
        if (IsKeyPressed(KEY_RIGHT)) {
            selecionado = selecionado->prox;
        } else if (IsKeyPressed(KEY_LEFT)) {
            selecionado = selecionado->ant;
        }
        // Botão Confirmar
        int confirmW = 260, confirmH = 56;
        int confirmX = w/2 - confirmW/2, confirmY = btnY + btnH + 50;
        Rectangle confirmBtn = {confirmX, confirmY, confirmW, confirmH};
        if (CheckCollisionPointRec(mouse, confirmBtn) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            currentMenuState = MENU_STATE_DONE;
        }
        // ENTER também confirma
        if (IsKeyPressed(KEY_ENTER)) {
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
    // Efeito Matrix
    float scale = 0.3f;
    int spriteW = 512 * scale;
    int spriteH = 512 * scale;
    int numCols = w / spriteW + 2;
    if (!matrixInited) {
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
        DrawTexturePro(matrixSprite, matrixFrames[currentFrame],
                       (Rectangle){x, y, spriteW, spriteH},
                       (Vector2){0, 0}, 0.0f, semiTransparentGreen);
    }
    // Sprite central: só na tela principal
    if (currentMenuState == MENU_STATE_MAIN) {
        float imgScale = 0.2f;
        float imgWidth = jogoTexture.width * imgScale;
        float imgHeight = jogoTexture.height * imgScale;
        float posX = (w - imgWidth) / 2;
        float posY = (h - imgHeight) / 2;
        DrawTextureEx(jogoTexture, (Vector2){posX, posY}, 0.0f, imgScale, WHITE);
    }
    if (currentMenuState == MENU_STATE_MAIN) {
        // Botão iniciar
        int btnW = 300;
        int btnH = 80;
        int btnX = w / 2 - btnW / 2;
        int btnY = h - btnH - 100;
        Rectangle btnRect = {btnX, btnY, btnW, btnH};
        Vector2 mouse = GetMousePosition();
        bool hovered = CheckCollisionPointRec(mouse, btnRect);
        Color btnColor = (hovered ? (Color){0, 200, 0, 255} : (Color){0, 180, 0, 255});
        DrawRectangleRec(btnRect, btnColor);
        const char *text = "Iniciar jogo";
        int fontSize = 36;
        int textW = MeasureText(text, fontSize);
        int textH = fontSize;
        int textX = btnX + (btnW - textW)/2;
        int textY = btnY + (btnH - textH)/2;
        DrawText(text, textX, textY, fontSize, WHITE);
    }
    else if (currentMenuState == MENU_STATE_CHOOSE_CHAR) {
        int spaceBetween = 20;
        int btnW = 160;
        int btnH = 80;
        int btnTotalW = numChars * btnW + (numChars-1) * spaceBetween;
        int btnStartX = (w - btnTotalW) / 2;
        int btnY = h / 2 - btnH / 2;
        int fontSize = 36;

        Vector2 mouse = GetMousePosition();
        Node *p = head;
        for (int i = 0; i < numChars; i++, p = p->prox) {
            Rectangle btnRect = {btnStartX + i*(btnW + spaceBetween), btnY, btnW, btnH};
            Color btnColor = BLACK;

            if (p == selecionado) {
                btnColor = (Color){40, 100, 40, 255};
            }
            // Hover (visual extra)
            if (CheckCollisionPointRec(mouse, btnRect)) {
                btnColor = (p == selecionado) ? (Color){60,160,60,255} : (Color){60,60,60,255};
            }
            DrawRectangleRec(btnRect, btnColor);
            int textW = MeasureText(p->nome, fontSize);
            DrawText(p->nome,
                btnRect.x + (btnW - textW)/2,
                btnRect.y + (btnH - fontSize)/2,
                fontSize,
                WHITE);
            DrawRectangleLinesEx(btnRect, 3, (p == selecionado) ? RAYWHITE : DARKGRAY);
        }
        DrawText("Escolha seu personagem", w/2 - 180, btnY - 70, 28, RAYWHITE);
        const char* instrText = "Clique em um personagem, use < > ou pressione ENTER";
        int instrFont = 24;
        int instrTextW = MeasureText(instrText, instrFont);
        DrawText(instrText, (w - instrTextW) / 2, btnY + btnH + 200, instrFont, GRAY);

        int confirmW = 260, confirmH = 56;
        int confirmX = w/2 - confirmW/2, confirmY = btnY + btnH + 50;
        Rectangle confirmBtn = {confirmX, confirmY, confirmW, confirmH};
        Color confirmColor = (CheckCollisionPointRec(mouse, confirmBtn)) ? (Color){0,200,0,255} : (Color){20,80,20,255};
        DrawRectangleRec(confirmBtn, confirmColor);
        const char* confText = "Confirmar";
        int confFont = 32;
        int confTw = MeasureText(confText, confFont);
        DrawText(confText, confirmX + (confirmW-confTw)/2, confirmY + (confirmH-confFont)/2, confFont, WHITE);
    }
    EndDrawing();
}

bool MenuStartGame(void)
{
    return (currentMenuState == MENU_STATE_DONE);
}

int MenuSelectedCharacter(void)
{
    // devolve índice (0=João, 1=Mateus, 2=Carlos, 3=Mamede)
    Node *p = head;
    int idx = 0;
    while (p && p != selecionado) {
        p = p->prox;
        idx++;
        if (p == head) break; // Segurança
    }
    return idx;
}

const char* MenuSelectedCharacterName(void)
{
    return nomeEscolhido;
}

void UnloadMenu(void)
{
    // Salva o nome escolhido antes de destruir a lista ligada!
    if (selecionado)
        strncpy(nomeEscolhido, selecionado->nome, sizeof(nomeEscolhido));
    else
        nomeEscolhido[0] = '\0';

    UnloadTexture(jogoTexture);
    UnloadTexture(matrixSprite);
    DestruirListaPersonagens();
}