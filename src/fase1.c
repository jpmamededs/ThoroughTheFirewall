#include "fase1.h"
#include "raylib.h"

static Texture2D empresa_bg;
static Texture2D pergunta_img;

void InitFase1(void) {
    empresa_bg = LoadTexture("src/sprites/empresa3.png");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
}

void UpdateFase1(void) {}

void DrawFase1(void) {
    BeginDrawing();
    ClearBackground(BLACK);

    // Fundo
    DrawTexturePro(
        empresa_bg,
        (Rectangle){0, 0, empresa_bg.width, empresa_bg.height},
        (Rectangle){0, 0, GetScreenWidth(), GetScreenHeight()},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

    // Caixa de diálogo (retângulo cinza)
    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;

    // Imagem da pergunta acima do retângulo
    int imgW = 1000;
    int imgH = pergunta_img.height - 130;
    int imgX = boxX;
    int imgY = boxY - imgH;

    DrawTexturePro(
        pergunta_img,
        (Rectangle){0, 0, pergunta_img.width, pergunta_img.height},
        (Rectangle){imgX, imgY, imgW, imgH},
        (Vector2){0, 0},
        0.0f,
        WHITE
    );

    // Texto "???" bem no canto inferior esquerdo da imagem
    int txtFontSize = 30;
    int txtX = imgX + 10;              // bem encostado à esquerda
    int txtY = imgY + imgH - txtFontSize;

    DrawText("???", txtX, txtY, txtFontSize, WHITE);

    // Balão de fala (retângulo com lado direito arredondado)
    int borderRadius = boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});

    // Texto da pergunta
    DrawText("Aqui será a pergunta?", boxX + 20, boxY + 30, 28, WHITE);

    EndDrawing();
}

void UnloadFase1(void) {
    UnloadTexture(empresa_bg);
    UnloadTexture(pergunta_img);
}
