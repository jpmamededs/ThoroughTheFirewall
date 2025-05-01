#include "raylib.h"

#define FRAME_COUNT 15

int main(void)
{
    int screenWidth = GetMonitorWidth(0);
    int screenHeight = GetMonitorHeight(0);

    InitWindow(screenWidth, screenHeight, "Sprite andando");
    SetWindowPosition(0, 0);
    InitAudioDevice();

    Music music = LoadMusicStream("src/music/EisenfunkPong-[AudioTrimmer.com] (1).mp3");
    PlayMusicStream(music);

    Sound welcomeSound = LoadSound("src/music/welcome-to-the-game-hacking-alert_sm4UxhuM.mp3");

    Texture2D sprite1 = LoadTexture("src/sprites/cropped.png");
    Texture2D sprite2 = LoadTexture("src/sprites/carinhaPdavida.png");
    Texture2D russia = LoadTexture("src/sprites/russia.png");
    Texture2D whiteHouse = LoadTexture("src/sprites/White_House.png");
    Texture2D hacker = LoadTexture("src/sprites/hackerscenery.jpg");
    Texture2D court = LoadTexture("src/sprites/courtscenery.jpeg");
    Texture2D hackerGuy = LoadTexture("src/sprites/hacker.png");
    Texture2D jogoTexture = LoadTexture("src/sprites/jogo.png");
    Texture2D matrixSprite = LoadTexture("src/sprites/Matrix.png");

    Rectangle matrixFrames[FRAME_COUNT] = {
        {0, 512, 512, 512},
        {0, 1024, 512, 512},
        {0, 1536, 512, 512},
        {512, 0, 512, 512},
        {512, 512, 512, 512},
        {512, 1024, 512, 512},
        {512, 1536, 512, 512},
        {1024, 0, 512, 512},
        {1024, 512, 512, 512},
        {1024, 1024, 512, 512},
        {1024, 1536, 512, 512},
        {1536, 0, 512, 512},
        {1536, 512, 512, 512},
        {1536, 1024, 512, 512},
        {1536, 1536, 512, 512}};

    int currentFrame = 0;
    float frameDuration = 0.05f;
    float lastFrameTime = 0.0f;

    SetTargetFPS(60);

    float time = 0.0f;
    float startTime = GetTime();
    bool firstSpriteDone = false;
    bool welcomePlayed = false;

    while (!WindowShouldClose())
    {
        UpdateMusicStream(music);
        time = GetTime() - startTime;

        float now = GetTime();
        if (time >= 26.5f && now - lastFrameTime >= frameDuration)
        {
            currentFrame = (currentFrame + 1) % FRAME_COUNT;
            lastFrameTime = now;
        }

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
        else if (time < 26.0f)
        {
            float animTime = time - 6.3f;

            Vector2 scale = {2.0f, 2.0f};
            float sprite1X = -sprite1.width * scale.x + (w + sprite1.width * scale.x) * (animTime / 10.0f);

            if (animTime >= 10.0f && !firstSpriteDone)
            {
                firstSpriteDone = true;
            }

            float sprite2X = w + sprite2.width * 3 - (w + sprite2.width * 3) * ((animTime - 2.0f) / 10.0f);
            Vector2 scale2 = {3.0f, 3.0f};
            Vector2 pos2 = {
                sprite2X,
                h / 2 - sprite2.height * scale2.y / 2 + 30};

            float hackerX = -hackerGuy.width * 1.5f + (w + hackerGuy.width * 1.5f) * ((animTime - 10.5f) / 10.0f);
            Vector2 scale3 = {1.2f, 1.2f};
            Vector2 pos3 = {
                hackerX,
                h / 2 - hackerGuy.height * scale3.y / 2};

            if (animTime >= 4.5f)
                DrawTextureEx(hackerGuy, pos3, 0.0f, scale3.x, WHITE);

            if (animTime >= 2.0f)
                DrawTextureEx(sprite2, pos2, 0.0f, scale2.x, WHITE);

            if (!firstSpriteDone)
                DrawTextureEx(sprite1, (Vector2){(int)sprite1X, (int)(h / 2 - sprite1.height / 2)}, 0.0f, scale.x, WHITE);

            DrawRectangle(0, 0, w, h / 4, BLACK);
            DrawRectangle(0, h - h / 4, w, h / 4, BLACK);
        }
        else if (time < 26.5f)
        {
            if (!welcomePlayed)
            {
                PlaySound(welcomeSound);
                welcomePlayed = true;
            }

            float alpha = (time - 26.0f) / 0.5f;
            if (alpha > 1.0f)
                alpha = 1.0f;

            ClearBackground(BLACK);

            float scale = 0.3f;
            int spriteW = 512 * scale;
            int spriteH = 512 * scale;
            int numCols = w / spriteW + 2;

            static float spriteY[64] = {0};
            static bool initialized = false;

            if (!initialized)
            {
                for (int i = 0; i < numCols; i++)
                    spriteY[i] = GetRandomValue(-spriteH, h);
                initialized = true;
            }

            float speed = 100.0f * GetFrameTime();

            for (int i = 0; i < numCols; i++)
            {
                spriteY[i] += speed;
                if (spriteY[i] > h)
                    spriteY[i] = -spriteH;

                float x = i * spriteW;
                float y = spriteY[i];

                DrawTexturePro(
                    matrixSprite,
                    matrixFrames[currentFrame],
                    (Rectangle){x, y, spriteW, spriteH},
                    (Vector2){0, 0}, 0.0f, WHITE);
            }
            float imgScale = 0.2f;
            float imgWidth = jogoTexture.width * imgScale;
            float imgHeight = jogoTexture.height * imgScale;
            float posX = (w - imgWidth) / 2;
            float posY = (h - imgHeight) / 2;
            DrawTextureEx(jogoTexture, (Vector2){posX, posY}, 0.0f, imgScale, WHITE);
            DrawRectangle(0, 0, w, h, (Color){255, 255, 255, (unsigned char)(255 * (1.0f - alpha))});
        }
        else
        {

            ClearBackground(BLACK);

            float scale = 0.3f;
            int spriteW = 512 * scale;
            int spriteH = 512 * scale;
            int numCols = w / spriteW + 2;

            static float spriteY[64] = {0};

            float speed = 100.0f * GetFrameTime();

            for (int i = 0; i < numCols; i++)
            {
                spriteY[i] += speed;
                if (spriteY[i] > h)
                    spriteY[i] = -spriteH;

                float x = i * spriteW;
                float y = spriteY[i];

                DrawTexturePro(
                    matrixSprite,
                    matrixFrames[currentFrame],
                    (Rectangle){x, y, spriteW, spriteH},
                    (Vector2){0, 0}, 0.0f, WHITE);
            }

            float imgScale = 0.2f;
            float imgWidth = jogoTexture.width * imgScale;
            float imgHeight = jogoTexture.height * imgScale;
            float posX = (w - imgWidth) / 2;
            float posY = (h - imgHeight) / 2;
            DrawTextureEx(jogoTexture, (Vector2){posX, posY}, 0.0f, imgScale, WHITE);
        }

        EndDrawing();
    }

    UnloadMusicStream(music);
    UnloadSound(welcomeSound);
    CloseAudioDevice();

    UnloadTexture(sprite1);
    UnloadTexture(sprite2);
    UnloadTexture(russia);
    UnloadTexture(whiteHouse);
    UnloadTexture(hacker);
    UnloadTexture(court);
    UnloadTexture(hackerGuy);
    UnloadTexture(jogoTexture);
    UnloadTexture(matrixSprite);

    CloseWindow();
    return 0;
}
