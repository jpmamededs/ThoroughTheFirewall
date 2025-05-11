#include "servidor_proxy.h"
#include "raylib.h"
#include <math.h>

static Model     modelo3D;
static Model     portaModel;
static Model     serverModel;
static Texture2D portaTexture;
static Texture2D serverTextures[3];
static Texture2D pc2dSprite;     // sprite 2D que aparece no HUD
static Sound     grabbingSound;   // som que toca quando o HUD aparece
static Music     footstepsMusic;  // som de passos em loop (streamed)
static Camera3D camera;
static float    cameraYaw      = 0.0f;
static Vector3  cameraPosition = { 0.0f, 1.6f, 0.0f };
static float    moveSpeed      = 10.0f;
static float cameraPitch = 0.0f; // Novo: Rotação para cima/baixo
static float sensitivity = 0.0022f; // Sensibilidade do mouse
static bool notebookFollow    = false;
static bool footstepsPlaying  = false;
static const float NOTEBOOK_SCALE = 0.05f;
static const Vector3 NOTEBOOK_POS = { 0.0f, -0.5f, -2.0f };
static Vector3 serverInitialPos;
static bool fase_concluida = false; 

// ———————————————————
// Funções utilitárias
// ———————————————————
static Vector3 AddVector3(Vector3 a, Vector3 b) {
    return (Vector3){ a.x + b.x, a.y + b.y, a.z + b.z };
}

static Vector3 ScaleVector3(Vector3 v, float scale) {
    return (Vector3){ v.x * scale, v.y * scale, v.z * scale };
}

static Vector3 NormalizeVector3(Vector3 v) {
    float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    if (length == 0.0f) return (Vector3){0.0f, 0.0f, 0.0f};
    return (Vector3){ v.x / length, v.y / length, v.z / length };
}

void UpdateFootstepsSound(bool moving) {
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
}

void UpdateCameraPosition() {
    camera.position = cameraPosition;
    Vector3 forward = NormalizeVector3((Vector3){
        cosf(cameraPitch) * sinf(cameraYaw),
        0.0f,
       -cosf(cameraPitch) * cosf(cameraYaw)
    });
    camera.target = AddVector3(camera.position, forward);
}

void UpdateCameraWithMouse(void)
{
    Vector2 mouseDelta = GetMouseDelta();

    // gira para a DIREITA quando o mouse vai para a direita
    cameraYaw += mouseDelta.x * sensitivity;
    // olha para cima/baixo
    cameraPitch -= mouseDelta.y * sensitivity;

    const float maxPitch = 85.0f * DEG2RAD;
    if (cameraPitch >  maxPitch) cameraPitch =  maxPitch;
    if (cameraPitch < -maxPitch) cameraPitch = -maxPitch;

    // forward normalizado  (-Z é “para a frente” no padrão OpenGL/Raylib)
    Vector3 forward = {
        cosf(cameraPitch) * sinf(cameraYaw),   // X
        sinf(cameraPitch),                    // Y
       -cosf(cameraPitch) * cosf(cameraYaw)    // Z
    };

    camera.target = AddVector3(camera.position, forward);
}

void UpdateMovement(float dt) {
    Vector3 forward = NormalizeVector3((Vector3){
        cosf(cameraPitch) * sinf(cameraYaw),
        0.0f,
       -cosf(cameraPitch) * cosf(cameraYaw)
    });

    Vector3 right = NormalizeVector3((Vector3){
        cosf(cameraYaw),
        0.0f,
        sinf(cameraYaw)
    });

    bool moving = false;
    if (IsKeyDown(KEY_W)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(forward, moveSpeed * dt));
        moving = true;
    }
    if (IsKeyDown(KEY_S)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(forward, -moveSpeed * dt));
        moving = true;
    }
    if (IsKeyDown(KEY_A)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(right, -moveSpeed * dt));
        moving = true;
    }
    if (IsKeyDown(KEY_D)) {
        cameraPosition = AddVector3(cameraPosition, ScaleVector3(right, moveSpeed * dt));
        moving = true;
    }

    UpdateFootstepsSound(moving);
}


void Init_ServidorProxy(void)
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
    SetMusicVolume(footstepsMusic, 15.0f);
    footstepsMusic.looping = true;

    // Configura câmera
    cameraYaw = 0.0f;
    cameraPitch = 0.0f; 
    camera.position   = cameraPosition;
    camera.target     = (Vector3){ 0.0f, 1.6f, -1.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f,  0.0f };
    camera.fovy       = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    DisableCursor();

    // Define posição fixa do servidor (mais longe)
    serverInitialPos = (Vector3){
        cameraPosition.x,
        cameraPosition.y - 1.8f,
        cameraPosition.z + 20.0f
    };

    fase_concluida = false;
}

void Update_ServidorProxy(void) {
    float dt = GetFrameTime();

    UpdateCameraWithMouse();
    UpdateMovement(dt);
    UpdateCameraPosition();
    UpdateMusicStream(footstepsMusic);

    // Ativa HUD e toca som ao coletar notebook
    if (!notebookFollow && (IsKeyDown(KEY_W) || IsKeyDown(KEY_S) || IsKeyDown(KEY_A) || IsKeyDown(KEY_D))) {
        notebookFollow = true;
        PlaySound(grabbingSound);
    }

    bool dentroAreaServidor =
        fabsf(cameraPosition.z - serverInitialPos.z) <= 5.0f &&
        fabsf(cameraPosition.x - serverInitialPos.x) <= 5.0f;

    if (dentroAreaServidor && IsKeyPressed(KEY_ENTER)) {
        fase_concluida = true;
        EnableCursor();
        PlaySound(grabbingSound);
    }
}

void Draw_ServidorProxy(void)
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

    // BOTÃO server
    const char *lbl = "Configurar Proxy [ENTER]";
    int fw = MeasureText(lbl, 20);
    int bw = fw + 20;         // padding 10px cada lado
    int bh = 48;              // um pouco mais alto p/ caber texto
    int bx = (GetScreenWidth()  - bw) / 2;
    int by = (GetScreenHeight() - bh) / 2;

    if (fabsf(cameraPosition.z - serverInitialPos.z) <= 5.0f &&
        fabsf(cameraPosition.x - serverInitialPos.x) <= 5.0f) {

        DrawRectangle(bx, by, bw, bh, DARKGRAY);
        DrawRectangleLines(bx, by, bw, bh, BLACK);
        DrawText(lbl, bx + 10, by + (bh - 20)/2, 20, WHITE);
    }

    EndDrawing();
}

bool Fase_ServidorProxy_Concluida(void)
{
    return fase_concluida;
}

void Unload_ServidorProxy(void)
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
