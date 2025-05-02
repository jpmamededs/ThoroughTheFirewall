#include "fase1.h"
#include "raylib.h"
#include <math.h>

static Model modelo3D;
static Texture2D pergunta_img;
static Sound somFase1;
static Camera3D camera;

static float cameraYaw = 0.0f;
static const float maxYaw = 45.0f * DEG2RAD;
static const float minYaw = -45.0f * DEG2RAD;

static bool somJaTocado = false;

void InitFase1(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");

    pergunta_img = LoadTexture("src/sprites/pergunta3.png");

    somFase1 = LoadSound("src/music/fase1-mateus.wav");
    somJaTocado = false;

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void UpdateFase1(void)
{
    if (!somJaTocado)
    {
        PlaySound(somFase1);
        somJaTocado = true;
    }

    if (IsKeyDown(KEY_LEFT))
        cameraYaw -= 0.02f;
    if (IsKeyDown(KEY_RIGHT))
        cameraYaw += 0.02f;

    if (cameraYaw > maxYaw)
        cameraYaw = maxYaw;
    if (cameraYaw < minYaw)
        cameraYaw = minYaw;

    float distance = 1.0f;
    camera.target.x = camera.position.x + sinf(cameraYaw) * distance;
    camera.target.z = camera.position.z - cosf(cameraYaw) * distance;
    camera.target.y = camera.position.y;
}

void DrawFase1(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    EndMode3D();

    int boxX = 60;
    int marginBottom = 220;
    int boxY = GetScreenHeight() - marginBottom;
    int boxWidth = GetScreenWidth() - 120;
    int boxHeight = 130;

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
        WHITE);

    int txtFontSize = 30;
    int txtX = imgX + 10;
    int txtY = imgY + imgH - txtFontSize;
    DrawText("???", txtX, txtY, txtFontSize, WHITE);

    int borderRadius = boxHeight / 2;
    DrawRectangle(boxX, boxY, boxWidth - borderRadius, boxHeight, (Color){20, 20, 20, 220});
    DrawCircle(boxX + boxWidth - borderRadius, boxY + borderRadius, borderRadius, (Color){20, 20, 20, 220});
    DrawText("Aqui será a pergunta?", boxX + 20, boxY + 30, 28, WHITE);

    EndDrawing();
}

void UnloadFase1(void)
{
    UnloadModel(modelo3D);
    UnloadTexture(pergunta_img);
    UnloadSound(somFase1);
}
