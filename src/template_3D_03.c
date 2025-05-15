#include "template_3D_03.h"
#include "raylib.h"
#include "generalFunctions.h"
#include <math.h>

// ESSA É A PARTE 2 DA FASE 3D DO SHELL REVERSO

static Model modelo3D;
static Model portaModel;
static Texture2D portaTexture;
static Camera3D camera;
static float cameraYaw = 0.0f;
static const float maxYaw = PI / 4.0f;
static const float minYaw = -PI / 4.0f;

static bool fase_concluida = false;
static Texture2D cdTexture;
static float cdPositionY = 0;
static float cdTimer = 0;
static bool cdVisible = false;
static bool cdReversing = false;
static bool showInsertButton = false;
static const float cdAppearTime = 1.5f;
static const float cdAppearSpeed = 4000.0f;
static const float cdReverseSpeed = 3500.0f;
static Sound insertSound;
static float soundTimer = 0;
static bool shouldPlaySound = false;
static bool soundPlayed = false;
static float completeTimer = 0; // Novo timer para esperar após o som
static bool waitingToComplete = false; // Flag para controlar a espera

void Init_Template_3D_03(void)
{
    modelo3D = LoadModel("src/models/old-computer.obj");
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;
    
    cdTexture = LoadTexture("src/sprites/cd.png");
    insertSound = LoadSound("src/music/insertingCd.mp3");
    
    camera.position = (Vector3){0.0f, 1.6f, 0.0f};
    camera.target = (Vector3){0.0f, 1.6f, -1.0f};
    camera.up = (Vector3){0.0f, 1.0f, 0.0f};
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();
    
    cdPositionY = GetScreenHeight() + cdTexture.height;
}

void Update_Template_3D_03(void)
{
    float mouseDeltaX = GetMouseDelta().x;
    cameraYaw += mouseDeltaX * 0.002f;

    if (cameraYaw > maxYaw) cameraYaw = maxYaw;
    if (cameraYaw < minYaw) cameraYaw = minYaw;

    float distance = 1.0f;
    camera.target.x = camera.position.x + sinf(cameraYaw) * distance;
    camera.target.z = camera.position.z - cosf(cameraYaw) * distance;
    camera.target.y = camera.position.y;
    
    cdTimer += GetFrameTime();
    
    if (!cdVisible && cdTimer >= cdAppearTime)
    {
        cdVisible = true;
    }
    
    if (cdVisible && !cdReversing && cdPositionY > GetScreenHeight() - cdTexture.height/2)
    {
        cdPositionY -= cdAppearSpeed * GetFrameTime();
        
        if (cdPositionY <= GetScreenHeight() - cdTexture.height/2)
        {
            cdPositionY = GetScreenHeight() - cdTexture.height/2;
            showInsertButton = true;
        }
    }
    
    if (showInsertButton && IsKeyPressed(KEY_SPACE))
    {
        showInsertButton = false;
        cdReversing = true;
    }
    
    if (cdReversing)
    {
        cdPositionY += cdReverseSpeed * GetFrameTime();
        
        if (cdPositionY >= GetScreenHeight() + cdTexture.height && !shouldPlaySound)
        {
            shouldPlaySound = true;
            soundTimer = 0;
        }
    }
    
    if (shouldPlaySound && !soundPlayed)
    {
        soundTimer += GetFrameTime();
        
        if (soundTimer >= 1.0f)
        {
            PlaySound(insertSound);
            shouldPlaySound = false;
            soundPlayed = true;
            waitingToComplete = true; // Ativa a espera para completar a fase
            completeTimer = 0; // Reseta o timer
        }
    }
    
    // Espera 1.5 segundos após o som tocar para completar a fase
    if (waitingToComplete)
    {
        completeTimer += GetFrameTime();
        
        if (completeTimer >= 2.5f)
        {
            fase_concluida = true;
            waitingToComplete = false;
        }
    }
}

void Draw_Template_3D_03(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);
    DrawModel(modelo3D, (Vector3){0.0f, -0.5f, -2.0f}, 0.05f, WHITE);
    DrawModelEx(portaModel, (Vector3){28.0f, -1.0f, -12.0f}, (Vector3){0, 1, 0}, 45.0f,
                (Vector3){0.05f, 0.05f, 0.05f}, WHITE);
    EndMode3D();
    
    if (cdVisible && !shouldPlaySound)
    {
        float cdPositionX = GetScreenWidth() - cdTexture.width - 20;
        DrawTexture(cdTexture, cdPositionX, cdPositionY, WHITE);
    }
    
    if (showInsertButton)
    {
        const char* text = "Pressione ESPACO para inserir disco";
        int fontSize = 20;
        Vector2 textSize = MeasureTextEx(GetFontDefault(), text, fontSize, 1);
        Vector2 textPos = {
            (GetScreenWidth() - textSize.x) / 2,
            GetScreenHeight() - 100
        };
        
        DrawTextEx(GetFontDefault(), text, textPos, fontSize, 1, WHITE);
    }

    EndDrawing();
}

void Unload_Template_3D_03(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadTexture(portaTexture);
    UnloadTexture(cdTexture);
    UnloadSound(insertSound);

    EnableCursor();
}

bool Fase_Template_3D_03_Concluida(void)
{
    return fase_concluida;
}