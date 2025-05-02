#include "fase1.h"
#include "raylib.h"
#include <math.h>

static Model modelo3D;
static Texture2D pergunta_img;
static Texture2D telefone_sprite;
static Sound somFase1;
static Sound somTelefone;
static Sound somRadio;
static Camera3D camera;

static float cameraYaw = 0.0f;
static const float maxYaw = 45.0f * DEG2RAD;
static const float minYaw = -45.0f * DEG2RAD;

static bool somFase1Tocado = false;
static bool somRadioTocado = false;
static bool interromperTelefone = false;
static bool telefoneVisivel = false;
static bool animandoTelefone = false;
static bool telefoneSubindo = false;
static bool animacaoFeita = false;
static float animacaoTelefoneY = 0.0f;
static float tempoDesdeInicio = 0.0f;
static float cooldownTelefone = -3.0f;
static Rectangle telefoneBounds = {0};

void InitFase1(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    pergunta_img = LoadTexture("src/sprites/pergunta3.png");
    telefone_sprite = LoadTexture("src/sprites/phone.png");

    somFase1 = LoadSound("src/music/fase1-mateus.wav");
    somTelefone = LoadSound("src/music/telefone.mp3");
    somRadio = LoadSound("src/music/phone-guy.mp3");

    SetSoundVolume(somFase1, 1.0f);
    SetSoundVolume(somTelefone, 1.0f);
    SetSoundVolume(somRadio, 1.0f);
    SetMasterVolume(1.0f);

    somFase1Tocado = false;
    somRadioTocado = false;
    interromperTelefone = false;
    telefoneVisivel = false;
    animandoTelefone = false;
    telefoneSubindo = false;
    animacaoFeita = false;
    animacaoTelefoneY = 0.0f;
    tempoDesdeInicio = 0.0f;
    cooldownTelefone = -3.0f;

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;
}

void UpdateFase1(void)
{
    float delta = GetFrameTime();
    tempoDesdeInicio += delta;

    if (!interromperTelefone)
    {
        cooldownTelefone += delta;
        if (!IsSoundPlaying(somTelefone) && cooldownTelefone >= 0.0f)
        {
            PlaySound(somTelefone);
            cooldownTelefone = -3.0f;
            telefoneVisivel = true;
            if (!animacaoFeita)
            {
                animandoTelefone = true;
                telefoneSubindo = true;
                animacaoTelefoneY = 1.0f;
            }
        }
    }

    Vector2 mouse = GetMousePosition();
    if (telefoneVisivel && CheckCollisionPointRec(mouse, telefoneBounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        interromperTelefone = true;
        StopSound(somTelefone);
        animandoTelefone = true;
        telefoneSubindo = false;
        animacaoTelefoneY = 0.0f;

        if (!somRadioTocado)
        {
            PlaySound(somRadio);
            somRadioTocado = true;
        }
    }

    if (tempoDesdeInicio >= 2.0f && !somFase1Tocado)
    {
        PlaySound(somFase1);
        somFase1Tocado = true;
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

    if (animandoTelefone)
    {
        float speed = 2.5f * delta;
        animacaoTelefoneY += (telefoneSubindo ? -speed : speed);

        if ((telefoneSubindo && animacaoTelefoneY <= 0.0f) || (!telefoneSubindo && animacaoTelefoneY >= 1.0f))
        {
            animacaoTelefoneY = telefoneSubindo ? 0.0f : 1.0f;
            animandoTelefone = false;
            animacaoFeita = true;
            if (!telefoneSubindo)
                telefoneVisivel = false;
        }
    }
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

    if (telefoneVisivel)
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        int spriteW = 389;
        int spriteH = 803;
        float scale = 1.25f;
        int destW = spriteW * scale;
        int destH = spriteH * scale * 0.75f;
        int destX = screenWidth - destW - 180;
        int destYFinal = screenHeight - destH;
        int deslocamentoY = (int)(animacaoTelefoneY * (screenHeight + destH));
        int destY = telefoneSubindo ? destYFinal + deslocamentoY : destYFinal + deslocamentoY;
        if (!animandoTelefone)
            destY = destYFinal;

        telefoneBounds = (Rectangle){destX, destY, destW, destH};

        DrawTexturePro(
            telefone_sprite,
            (Rectangle){0, 0, spriteW, spriteH * 0.75f},
            telefoneBounds,
            (Vector2){0, 0},
            0.0f,
            WHITE);
    }

    EndDrawing();
}

void UnloadFase1(void)
{
    UnloadModel(modelo3D);
    UnloadTexture(pergunta_img);
    UnloadTexture(telefone_sprite);
    UnloadSound(somFase1);
    UnloadSound(somTelefone);
    UnloadSound(somRadio);
}