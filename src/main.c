#include "raylib.h"

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);

    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);
    InitAudioDevice();

    Music music = LoadMusicStream("src/music/EisenfunkPong.mp3");
    PlayMusicStream(music);

    Texture2D sprite1 = LoadTexture("src/sprites/cropped.png");
    Texture2D sprite2 = LoadTexture("src/sprites/carinhaPdavida.png");

    SetTargetFPS(60);

    float time = 0.0f;
    bool firstSpriteDone = false;
    float sprite1StartTime = 0.0f;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        time = GetTime();

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        Vector2 scale = {2.0f, 2.0f};

        float sprite1X = -sprite1.width * 2 + (w + sprite1.width * 2) * (time / 10.0f);

        if (time >= 10.0f && !firstSpriteDone) {
            firstSpriteDone = true;
            sprite1StartTime = time;
        }

        float sprite2X = w + sprite2.width * 3 - (w + sprite2.width * 3) * ((time - sprite1StartTime) / 10.0f);
        Vector2 scale2 = {3.0f, 3.0f};
        Vector2 pos2 = {
            sprite2X,
            h / 2 - sprite2.height * scale2.y / 2 + 30
        };

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!firstSpriteDone) {
            DrawTextureEx(sprite1, (Vector2){(int)sprite1X, (int)(h / 2 - sprite1.height / 2)}, 0.0f, scale.x, WHITE);
        }
        if (firstSpriteDone) {
            DrawTextureEx(sprite2, pos2, 0.0f, scale2.x, WHITE);
        }

        DrawRectangle(0, 0, w, h / 4, BLACK);                     
        DrawRectangle(0, h - h / 4, w, h / 4, BLACK);            

        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    UnloadTexture(sprite1);
    UnloadTexture(sprite2);
    CloseWindow();

    return 0;
}
