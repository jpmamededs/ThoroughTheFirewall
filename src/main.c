#include "raylib.h"

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);

    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);
    InitAudioDevice();

    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);

    Texture2D sprite1 = LoadTexture("src/sprites/cropped.png");
    Texture2D sprite2 = LoadTexture("src/sprites/carinhaPdavida.png");
    Texture2D russia = LoadTexture("src/sprites/russia.png");
    Texture2D whiteHouse = LoadTexture("src/sprites/White_House.png");
    Texture2D hacker = LoadTexture("src/sprites/hackerscenery.jpg");
    Texture2D court = LoadTexture("src/sprites/courtscenery.jpeg");

    SetTargetFPS(60);

    float time = 0.0f;
    float startTime = GetTime();
    bool firstSpriteDone = false;
    float sprite1StartTime = 0.0f;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        time = GetTime() - startTime;

        int w = GetScreenWidth();
        int h = GetScreenHeight();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (time < 1.2f)
        {
            DrawTexturePro(russia, (Rectangle){0, 0, russia.width, russia.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (time < 1.7f)
        {
            float alpha = (time - 1.2f) / 0.5f;
            Color fadeOut = (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))};
            Color fadeIn = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
            DrawTexturePro(russia, (Rectangle){0, 0, russia.width, russia.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeOut);
            DrawTexturePro(whiteHouse, (Rectangle){0, 0, whiteHouse.width, whiteHouse.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeIn);
            DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
        }
        else if (time < 2.9f)
        {
            DrawTexturePro(whiteHouse, (Rectangle){0, 0, whiteHouse.width, whiteHouse.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (time < 3.4f)
        {
            float alpha = (time - 2.9f) / 0.5f;
            Color fadeOut = (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))};
            Color fadeIn = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
            DrawTexturePro(whiteHouse, (Rectangle){0, 0, whiteHouse.width, whiteHouse.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeOut);
            DrawTexturePro(hacker, (Rectangle){0, 0, hacker.width, hacker.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeIn);
            DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
        }
        else if (time < 4.6f)
        {
            DrawTexturePro(hacker, (Rectangle){0, 0, hacker.width, hacker.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (time < 5.1f)
        {
            float alpha = (time - 4.6f) / 0.5f;
            Color fadeOut = (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))};
            Color fadeIn = (Color){255, 255, 255, (unsigned char)(255 * alpha)};
            DrawTexturePro(hacker, (Rectangle){0, 0, hacker.width, hacker.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeOut);
            DrawTexturePro(court, (Rectangle){0, 0, court.width, court.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, fadeIn);
            DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
        }
        else if (time < 6.3f)
        {
            DrawTexturePro(court, (Rectangle){0, 0, court.width, court.height}, (Rectangle){0, 0, w, h}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else
        {
            float animTime = time - 6.3f;

            Vector2 scale = {2.0f, 2.0f};
            float sprite1X = -sprite1.width * scale.x + (w + sprite1.width * scale.x) * (animTime / 10.0f);

            if (animTime >= 10.0f && !firstSpriteDone)
            {
                firstSpriteDone = true;
                sprite1StartTime = animTime;
            }

            float sprite2X = w + sprite2.width * 3 - (w + sprite2.width * 3) * ((animTime - sprite1StartTime) / 10.0f);
            Vector2 scale2 = {3.0f, 3.0f};
            Vector2 pos2 = {
                sprite2X,
                h / 2 - sprite2.height * scale2.y / 2 + 30};

            if (!firstSpriteDone)
                DrawTextureEx(sprite1, (Vector2){(int)sprite1X, (int)(h / 2 - sprite1.height / 2)}, 0.0f, scale.x, WHITE);

            if (firstSpriteDone)
                DrawTextureEx(sprite2, pos2, 0.0f, scale2.x, WHITE);

            DrawRectangle(0, 0, w, h / 4, BLACK);
            DrawRectangle(0, h - h / 4, w, h / 4, BLACK);
        }

        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    UnloadTexture(sprite1);
    UnloadTexture(sprite2);
    UnloadTexture(russia);
    UnloadTexture(whiteHouse);
    UnloadTexture(hacker);
    UnloadTexture(court);
    CloseWindow();

    return 0;
}
