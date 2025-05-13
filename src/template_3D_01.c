#include "template_3D_01.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>

// ESSE É KEYLOGGER 3D

static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Texture2D phoneTexture;
static Camera3D camera;
static float cameraYaw = 0.0f;
static const float maxYaw = PI / 4.0f;
static const float minYaw = -PI / 4.0f;

static bool fase4_concluida = false;
static float timeElapsed = 0.0f;

static float phonePosY = 0.0f;
static bool phoneVisible = false;
static bool animationComplete = false;
static bool reverseAnimation = false; // Controle da animação reversa
static const float phoneSpeed = 2000.0f; // Velocidade rápida da subida

void Init_Template_3D_01(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;
    phoneTexture = LoadTexture("src/sprites/phone.png");

    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();

    // Posição inicial fora da tela (borda inferior)
    phonePosY = GetScreenHeight();
}

void Update_Template_3D_01(void)
{
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;

    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;

    float distance = 1.0f;
    camera.target.x = camera.position.x + sinf(cameraYaw) * distance;
    camera.target.z = camera.position.z - cosf(cameraYaw) * distance;
    camera.target.y = camera.position.y;

    // Atualizar o tempo para a exibição do telefone
    timeElapsed += GetFrameTime();
    if (timeElapsed > 3.0f) phoneVisible = true;

    // Verificar se a tecla A foi pressionada para iniciar a animação reversa
    if (IsKeyPressed(KEY_A) && animationComplete)
    {
        reverseAnimation = true;
    }

    // Animação de subida do telefone (de baixo para cima)
    if (phoneVisible && !animationComplete && !reverseAnimation)
    {
        phonePosY -= phoneSpeed * GetFrameTime();

        // Posição final para que apenas 4/5 do sprite sejam visíveis
        int screenHeight = GetScreenHeight();
        int phoneHeight = phoneTexture.height;
        int finalPosY = screenHeight - (phoneHeight * (4.0f / 5.0f));

        if (phonePosY <= finalPosY)
        {
            phonePosY = finalPosY;
            animationComplete = true;
        }
    }

    // Animação reversa (descendo)
    if (reverseAnimation && phonePosY < GetScreenHeight())
    {
        phonePosY += phoneSpeed * GetFrameTime();

        if (phonePosY >= GetScreenHeight()) 
        {
            phonePosY = GetScreenHeight();
            reverseAnimation = false; // Animação reversa completa
        }
    }
}

void Draw_Template_3D_01(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    DrawModelEx(portaModel, (Vector3){28.0f, -1.0f, -12.0f}, (Vector3){0, 1, 0}, 45.0f,
                (Vector3){0.05f, 0.05f, 0.05f}, WHITE);
    EndMode3D();

    // Desenhar o telefone após 3 segundos (subida rápida da borda inferior)
    if (phoneVisible)
    {
        int screenWidth = GetScreenWidth();
        float scale = 1.0f;

        int phoneWidth = phoneTexture.width * scale;

        // Posição do telefone próximo ao canto inferior direito, mas um pouco mais à esquerda
        int posX = screenWidth - phoneWidth - 60;

        // Desenhar o sprite completo na posição final
        DrawTexture(phoneTexture, posX, (int)phonePosY, WHITE);
    }

    EndDrawing();
}

void Unload_Template_3D_01(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadTexture(phoneTexture);

    EnableCursor();
}

bool Fase_Template_3D_01_Concluida(void)
{
    return fase4_concluida;
}
