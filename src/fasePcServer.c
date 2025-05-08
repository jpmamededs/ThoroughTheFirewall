#include "fasePCServer.h"
#include "raylib.h"
#include <math.h>

// ————————————————
// Funções utilitárias 3D
// ————————————————
static Vector3 AddVector3(Vector3 a, Vector3 b) {
    return (Vector3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

// ————————————————
// Modelos, Texturas e Sprite HUD
// ————————————————
static Model     modelo3D;
static Model     portaModel;
static Model     serverModel;
static Texture2D portaTexture;
static Texture2D serverTextures[3];
static Texture2D pc2dSprite;     // sprite 2D que aparece no HUD
static Sound     grabbingSound;   // som que toca quando o HUD aparece
static Music     footstepsMusic;  // som de passos em loop (streamed)

// ————————————————
// Câmera e Movimento
// ————————————————
static Camera3D camera;
static float    cameraYaw      = 0.0f;
static Vector3  cameraPosition = { 0.0f, 1.6f, 0.0f };
static float    moveSpeed      = 10.0f;

// ————————————————
// Estado de HUD & passos
// ————————————————
static bool notebookFollow    = false;
static bool footstepsPlaying  = false;

// ————————————————
// Notebook inicial em 3D
// ————————————————
static const float NOTEBOOK_SCALE = 0.05f;
static const Vector3 NOTEBOOK_POS = { 0.0f, -0.5f, -2.0f };

// ————————————————
// Servidor fixo atrás (mais “afundo”)
// ————————————————
static Vector3 serverInitialPos;

void InitFasePCServer(void)
{
    // Carrega notebook 3D
    modelo3D = LoadModel("src/models/old-computer.obj");

    // Carrega porta 3D
    portaModel = LoadModel("src/models/DOOR.obj");
    portaTexture = LoadTexture("src/models/Garage_Metalness.png");
    portaModel.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = portaTexture;

    // Carrega servidor 3D
    serverModel = LoadModel("src/models/server computer.obj");
    serverTextures[0] = LoadTexture("src/models/serverfront.png");
    serverTextures[1] = LoadTexture("src/models/server top, bottom, side.jpg");
    serverTextures[2] = LoadTexture("src/models/Server_back side.jpg");
    for (int i = 0; i < serverModel.materialCount; i++) {
        serverModel.materials[i].maps[MATERIAL_MAP_DIFFUSE].texture = serverTextures[i];
    }

    // Carrega sprite 2D do notebook
    pc2dSprite = LoadTexture("src/sprites/pc2d.png");

    // Carrega sons
    grabbingSound   = LoadSound("src/music/grabbing.mp3");
    footstepsMusic  = LoadMusicStream("src/music/footsteps.mp3");
    footstepsMusic.looping = true;

    // Configura câmera
    camera.position   = cameraPosition;
    camera.target     = (Vector3){ 0.0f, 1.6f, -1.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f,  0.0f };
    camera.fovy       = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    // Define posição fixa do servidor (mais longe)
    serverInitialPos = (Vector3){
        cameraPosition.x,
        cameraPosition.y - 1.8f,
        cameraPosition.z + 20.0f
    };
}

void UpdateFasePCServer(void)
{
    float dt = GetFrameTime();

    // Atualiza stream de passos
    UpdateMusicStream(footstepsMusic);

    // Rotaciona câmera (yaw)
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A)) cameraYaw -= 3.0f * dt;
    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) cameraYaw += 3.0f * dt;

    // Calcula forward & right
    Vector3 forward = { sinf(cameraYaw), 0.0f, -cosf(cameraYaw) };
    Vector3 right   = { forward.z,      0.0f, -forward.x };

    // Move o jogador (câmera)
    bool moving = false;
    if (IsKeyDown(KEY_W)) { cameraPosition = AddVector3(cameraPosition, (Vector3){ forward.x*moveSpeed*dt, 0, forward.z*moveSpeed*dt }); moving = true; }
    if (IsKeyDown(KEY_S)) { cameraPosition = AddVector3(cameraPosition, (Vector3){-forward.x*moveSpeed*dt, 0,-forward.z*moveSpeed*dt }); moving = true; }
    if (IsKeyDown(KEY_Q)) { cameraPosition = AddVector3(cameraPosition, (Vector3){ right.x*moveSpeed*dt,   0, right.z*moveSpeed*dt   }); moving = true; }
    if (IsKeyDown(KEY_E)) { cameraPosition = AddVector3(cameraPosition, (Vector3){-right.x*moveSpeed*dt,   0,-right.z*moveSpeed*dt   }); moving = true; }

    // Controla som de passos em loop
    if (moving) {
        if (!footstepsPlaying) {
            PlayMusicStream(footstepsMusic);
            footstepsPlaying = true;
        }
    } else {
        if (footstepsPlaying) {
            StopMusicStream(footstepsMusic);
            footstepsPlaying = false;
        }
    }

    // Atualiza posição da câmera
    camera.position = cameraPosition;
    camera.target   = (Vector3){
        cameraPosition.x + forward.x,
        cameraPosition.y,
        cameraPosition.z + forward.z
    };

    // Ativa HUD e toca grabbing na primeira movimentação
    if (!notebookFollow && moving) {
        notebookFollow = true;
        PlaySound(grabbingSound);
    }
}

void DrawFasePCServer(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(camera);

    // Notebook 3D enquanto estática
    if (!notebookFollow) {
        DrawModel(modelo3D, NOTEBOOK_POS, NOTEBOOK_SCALE, WHITE);
    }

    // Porta 3D
    DrawModelEx(portaModel,
                (Vector3){28.0f, -1.0f, -12.0f},
                (Vector3){0.0f, 1.0f, 0.0f},
                45.0f,
                (Vector3){0.05f, 0.05f, 0.05f},
                WHITE);

    // Servidor 3D (mais longe)
    DrawModelEx(serverModel,
                serverInitialPos,
                (Vector3){0.0f, 1.0f, 0.0f},
                180.0f,
                (Vector3){0.01f, 0.01f, 0.01f},
                WHITE);

    EndMode3D();

    // HUD 2D após movimentar
    if (notebookFollow) {
        int x = -500;
        int y = GetScreenHeight() - pc2dSprite.height - 10;
        DrawTextureEx(pc2dSprite, (Vector2){(float)x, (float)y}, 5.0f, 1.0f, WHITE);
    }

    // Se o jogador estiver em z <= -20 e x entre -6 e 6, mostra botão
    if (cameraPosition.z <= -20.0f && cameraPosition.x >= -6.0f && cameraPosition.x <= 6.0f) {
        const int bw = 180, bh = 40;
        int bx = (GetScreenWidth() - bw)/2 - 200;
        int by = GetScreenHeight()/2 - bh/2;
        DrawRectangle(bx, by, bw, bh, DARKGRAY);
        DrawRectangleLines(bx, by, bw, bh, BLACK);
        DrawText("Configurar Proxy", bx + 10, by + 10, 20, WHITE);
    }

    EndDrawing();
}

void UnloadFasePCServer(void)
{
    UnloadModel(modelo3D);
    UnloadModel(portaModel);
    UnloadModel(serverModel);

    UnloadTexture(portaTexture);
    for (int i = 0; i < 3; i++) UnloadTexture(serverTextures[i]);
    UnloadTexture(pc2dSprite);

    UnloadSound(grabbingSound);
    UnloadMusicStream(footstepsMusic);
}
