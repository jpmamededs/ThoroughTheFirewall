#include "template_3D_02.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>
#include <stdio.h>

// Variáveis Globais
static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Sound doorKnockSound;  // Som da batida na porta
static Camera3D camera;
static float cameraYaw = 0.0f;
static const float maxYaw = PI / 4.0f;
static const float minYaw = -PI / 4.0f;

static bool fase4_concluida = false;
static float timeElapsed = 0.0f;
static bool soundPlaying = false;
static bool doorAnswered = false;
static bool showButton = false;

void Init_Template_3D_02(void)
{
    // Inicializar Modelos e Texturas
    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

    doorKnockSound = LoadSound("src/music/doorKnock.mp3");
    SetSoundVolume(doorKnockSound, 1.0f);  // Volume moderado

    // Inicializar a câmera
    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();
}

bool IsLookingAtDoor()
{
    // Verifica se o ângulo está entre 30° e 50° para a direita
    float angleMin = 30 * DEG2RAD;
    float angleMax = 50 * DEG2RAD;

    return (cameraYaw > angleMin && cameraYaw < angleMax);
}

void Update_Template_3D_02(void)
{
    float deltaTime = GetFrameTime();
    timeElapsed += deltaTime;

    // Tocar o som em loop quando iniciar (após 2 segundos)
    if (timeElapsed >= 2.0f && !doorAnswered) 
    {
        if (!soundPlaying)
        {
            PlaySound(doorKnockSound);
            soundPlaying = true;
        }

        // Verificar se está olhando para o ângulo correto
        if (IsLookingAtDoor())
        {
            showButton = true;
        }
        else
        {
            showButton = false;
        }

        // Parar o som se a porta for atendida (pressionar SPACE)
        if (showButton && IsKeyPressed(KEY_SPACE))
        {
            StopSound(doorKnockSound);
            doorAnswered = true;
            soundPlaying = false;
            showButton = false;
        }

        // Reiniciar o som se não estiver tocando
        if (!IsSoundPlaying(doorKnockSound))
        {
            PlaySound(doorKnockSound);
        }
    }

    // Controle da câmera
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;

    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;

    float distance = 1.0f;
    camera.target.x = camera.position.x + sinf(cameraYaw) * distance;
    camera.target.z = camera.position.z - cosf(cameraYaw) * distance;
    camera.target.y = camera.position.y;
}

void Draw_Template_3D_02(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    DrawModelEx(portaModel, (Vector3){28.0f, -1.0f, -12.0f}, (Vector3){0, 1, 0}, 45.0f,
                (Vector3){0.05f, 0.05f, 0.05f}, WHITE);
    EndMode3D();

    // Botão "Atender Porta" (Verde e com tecla SPACE)
    if (showButton)
    {
        int screenWidth = GetScreenWidth();
        int screenHeight = GetScreenHeight();
        const char *buttonText = "Atender Porta [SPACE]";
        int textWidth = MeasureText(buttonText, 30);

        // Botão verde
        DrawRectangle(screenWidth / 2 - textWidth / 2 - 10, screenHeight - 80, textWidth + 20, 50, GREEN);
        DrawText(buttonText, screenWidth / 2 - textWidth / 2, screenHeight - 70, 30, BLACK);
    }

    EndDrawing();
}

void Unload_Template_3D_02(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadSound(doorKnockSound);
    CloseAudioDevice();
    EnableCursor();
}

bool Fase_Template_3D_02_Concluida(void)
{
    return fase4_concluida;
}
