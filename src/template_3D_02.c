#include "template_3D_02.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>
#include <stdio.h>

// Variáveis Globais
static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Texture2D closedBoxTexture;  // Novo: Textura da caixa fechada
static Sound doorKnockSound;
static Sound doorOpeningSound;
static Camera3D camera;
static float cameraYaw = 0.0f;
static const float maxYaw = PI / 4.0f;
static const float minYaw = -PI / 4.0f;

static bool template3D_02_concluida = false;
static float timeElapsed = 0.0f;
static bool soundPlaying = false;
static bool doorAnswered = false;

// Variáveis do Flash e Fade
static float fadeAlpha = 0.0f;
static bool isFading = false;
static bool fadeComplete = false;
static float fadeDuration = 2.5f;
static float fadeSpeed;
static float flashHoldTime = 1.0f;
static float flashTimer = 0.0f;
static bool flashActive = false;
static bool showBox = false;  // Novo: Indica quando mostrar a caixa fechada

void Init_Template_3D_02(void)
{
    fadeSpeed = 1.0f / fadeDuration;

    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    closedBoxTexture = LoadTexture("src/sprites/closed_cardboardBox.png");  // Carregar a caixa

    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

    doorKnockSound = LoadSound("src/music/doorKnock.mp3");
    doorOpeningSound = LoadSound("src/music/doorOpening.mp3");
    SetSoundVolume(doorKnockSound, 1.0f);
    SetSoundVolume(doorOpeningSound, 1.0f);

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();
}

void StartFadeOut()
{
    isFading = true;
    fadeAlpha = 1.0f;
    fadeComplete = false;
    flashTimer = 0.0f;
    flashActive = true;
}

void UpdateFadeOut(float deltaTime)
{
    if (flashActive)
    {
        flashTimer += deltaTime;

        // Fase 1: Flash Branco Intenso
        if (flashTimer < flashHoldTime)
        {
            fadeAlpha = 1.0f;
        }
        else
        {
            // Fase 2: Transição para a Tela Preta (Fade Out)
            fadeAlpha -= fadeSpeed * deltaTime;
            if (fadeAlpha <= 0.0f)
            {
                fadeAlpha = 0.0f;
                isFading = false;
                fadeComplete = true;
                flashActive = false;

                // Ativar a exibição da caixa fechada após o fade
                showBox = true;

                // Tocar o som após o fade estar completo
                PlaySound(doorOpeningSound);
            }
        }
    }
}

void Update_Template_3D_02(void)
{
    float deltaTime = GetFrameTime();
    timeElapsed += deltaTime;

    UpdateFadeOut(deltaTime);

    if (fadeComplete)
    {
        template3D_02_concluida = true;
        return;
    }

    if (!isFading && timeElapsed >= 2.0f && !doorAnswered) 
    {
        if (!soundPlaying)
        {
            PlaySound(doorKnockSound);
            soundPlaying = true;
        }

        if (IsKeyPressed(KEY_SPACE))
        {
            if (IsSoundPlaying(doorKnockSound))
            {
                StopSound(doorKnockSound);
                soundPlaying = false;
            }

            doorAnswered = true;
            StartFadeOut();
        }
    }

    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;
    cameraYaw = fmaxf(fminf(cameraYaw, maxYaw), minYaw);

    camera.target.x = camera.position.x + sinf(cameraYaw);
    camera.target.z = camera.position.z - cosf(cameraYaw);
}

void Draw_Template_3D_02(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    if (!flashActive)  // Apenas desenha os modelos se o flash não estiver ativo
    {
        BeginMode3D(camera);
        DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
        DrawModelEx(portaModel, (Vector3){28.0f, -1.0f, -12.0f}, (Vector3){0, 1, 0}, 45.0f, (Vector3){0.05f, 0.05f, 0.05f}, WHITE);
        EndMode3D();
    }

    // Desenhar o efeito de flash branco seguido de tela preta
    if (flashActive || isFading)
    {
        if (flashTimer < flashHoldTime)
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
        }
        else
        {
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), BLACK);
            if (showBox)
            {
                float scale = 0.8f;
                int boxWidth = closedBoxTexture.width * scale;
                int boxHeight = closedBoxTexture.height * scale;
                int posX = (GetScreenWidth() - boxWidth) / 2;
                int posY = (GetScreenHeight() - boxHeight) / 2;

                DrawTextureEx(closedBoxTexture, (Vector2){posX, posY}, 0.0f, scale, WHITE);
            }
        }
    }

    EndDrawing();
}

void Unload_Template_3D_02(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadTexture(closedBoxTexture);  // Descarregar a caixa
    UnloadSound(doorKnockSound);
    UnloadSound(doorOpeningSound);
    CloseAudioDevice();
    EnableCursor();
}

bool Fase_Template_3D_02_Concluida(void)
{
    return template3D_02_concluida;
}
