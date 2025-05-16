#include "tela_provisoria_02.h"
#include "raylib.h"
#include "generalFunctions.h"
#include "playerStats.h"

// ESSA É A TELA DO RESULTADO DO PROCESSO SELETIVO

static bool fase_concluida = false;

// Texturas
static Texture2D carinhaRevelationSilhouette;
static Texture2D carinhaRevelation;
static Texture2D meninaRevelationSilhouette;
static Texture2D meninaRevelation;
static Texture2D calvoRevelationSilhouette;
static Texture2D calvoRevelation;
static Texture2D deBoneRevelationSilhouette;
static Texture2D deBoneRevelation;

static Texture2D cybertechShield;
// Timer para controlar o aparecimento do escudo
static float shieldAppearTimer = 0.0f;
static bool shieldAppeared = false;

// Áudio
static Music writtenInTheStars;
static Sound surpriseSound; // Novo som para o efeito de flash
static Sound congrats;

// Variáveis de controle
static float parallaxTimer = 0.0f;
static const float parallaxDuration = 13.0f;
static bool movimentoFinalizado = false;
static float parallaxProgress = 0.0f;
static bool surprisePlayed = false; // Para controlar se o som já foi tocado

// Posições finais
static const int initialPos = 700;
static const int finalPosMenina = 800;
static const int finalPosCalvo = 900;
static const int finalPosDeBone = 1000;

// Variáveis para animação
static bool spriteTrocaRealizada = false;
static bool deBoneTrocaRealizada = false;
static bool meninaTrocaRealizada = false;
static bool calvoTrocaRealizada = false;

// Variáveis para efeito de flash
static float flashAlpha = 0.0f;
static bool flashAtivo = false;
static const float flashDuration = 0.6f;
static float flashTimer = 0.0f;

void Init_Tela_02(void)
{
    fase_concluida = false;

    // Carregar texturas
    carinhaRevelationSilhouette = LoadTexture("src/sprites/revelation/carinhaRevelationSilhouette.png");
    carinhaRevelation = LoadTexture("src/sprites/revelation/carinhaRevelation.png");
    meninaRevelationSilhouette = LoadTexture("src/sprites/revelation/meninaRevelationSilhouette.png");
    meninaRevelation = LoadTexture("src/sprites/revelation/meninaRevelation.png");
    calvoRevelationSilhouette = LoadTexture("src/sprites/revelation/calvoRevelationSilhouette.png");
    calvoRevelation = LoadTexture("src/sprites/revelation/calvoRevelation.png");
    deBoneRevelationSilhouette = LoadTexture("src/sprites/revelation/deBoneRevelationSilhouette.png");
    deBoneRevelation = LoadTexture("src/sprites/revelation/deBoneRevelation.png");
    cybertechShield = LoadTexture("src/sprites/revelation/cybertechShield.png");
    congrats = LoadSound("src/music/congrats.wav");

    // Carregar áudio
    writtenInTheStars = LoadMusicStream("src/music/writtenInTheStars.mp3");
    surpriseSound = LoadSound("src/music/surprise.mp3"); // Carregar o som de surpresa
    PlayMusicStream(writtenInTheStars);
    SetMusicVolume(writtenInTheStars, 0.5f);
    PlaySound(congrats);

    // Inicializar variáveis
    parallaxTimer = 0.0f;
    parallaxProgress = 0.0f;
    movimentoFinalizado = false;
    spriteTrocaRealizada = false;
    deBoneTrocaRealizada = false;
    meninaTrocaRealizada = false;
    calvoTrocaRealizada = false;
    surprisePlayed = false;
}

void Update_Tela_02(void)
{
    if (IsKeyPressed(KEY_ENTER))
    {
        fase_concluida = true;
    }

    UpdateMusicStream(writtenInTheStars);

    // Atualizar movimento parallax
    if (!movimentoFinalizado)
    {
        parallaxTimer += GetFrameTime();
        parallaxProgress = parallaxTimer / parallaxDuration;

        if (parallaxProgress >= 1.0f)
        {
            parallaxProgress = 1.0f;
            movimentoFinalizado = true;

            // Ativar flash e troca de sprites quando o movimento terminar
            flashAtivo = true;
            flashTimer = 0.0f;
            surprisePlayed = false; // Resetar para poder tocar o som novamente

            if (strcmp(gSelectedCharacterName, "Dante") == 0)
            {
                spriteTrocaRealizada = true;
            }
            else if (strcmp(gSelectedCharacterName, "Jade") == 0)
            {
                deBoneTrocaRealizada = true;
            }
            else if (strcmp(gSelectedCharacterName, "Alice") == 0)
            {
                meninaTrocaRealizada = true;
            }
            else if (strcmp(gSelectedCharacterName, "Levi") == 0)
            {
                calvoTrocaRealizada = true;
            }
        }
    }

    // Efeito de flash
    if (flashAtivo)
    {
        // Tocar o som de surpresa apenas uma vez quando o flash começar
        if (!surprisePlayed)
        {
            PlaySound(surpriseSound);
            flashAlpha = 1.0f; // Começa imediatamente com flash total
            surprisePlayed = true;
        }

        flashTimer += GetFrameTime();

        // Apenas fade out na segunda metade da duração
        if (flashTimer >= flashDuration / 2)
        {
            flashAlpha = 1.0f - (flashTimer - flashDuration / 2) * 2.0f / flashDuration;
        }

        if (flashTimer >= flashDuration)
        {
            flashAtivo = false;
            flashAlpha = 0.0f;
        }

        if (!shieldAppeared)
        {
            shieldAppearTimer += GetFrameTime();
            if (shieldAppearTimer >= 16.0f)
            {
                shieldAppeared = true;
                shieldAppearTimer = 0.0f;
            }
        }
    }
}

void Draw_Tela_02(void)
{
    BeginDrawing();
    ClearBackground(BLACK);

    int screenHeight = GetScreenHeight();

    // Fatores de escala
    const float scaleCarinha = 0.4f;
    const float scaleMenina = 0.6f;
    const float scaleCalvo = 0.8f;
    const float scaleDeBone = 1.0f;

    // Calcular posições atuais com base no progresso
    int xMenina = initialPos + (finalPosMenina - initialPos) * parallaxProgress;
    int xCalvo = initialPos + (finalPosCalvo - initialPos) * parallaxProgress;
    int xDeBone = initialPos + (finalPosDeBone - initialPos) * parallaxProgress;

    // Posições Y (altura)
    int yCarinha = screenHeight - carinhaRevelationSilhouette.height * scaleCarinha;
    int yMenina = screenHeight - meninaRevelationSilhouette.height * scaleMenina;
    int yCalvo = screenHeight - calvoRevelationSilhouette.height * scaleCalvo;
    int yDeBone = screenHeight - deBoneRevelationSilhouette.height * scaleDeBone;

    // DESENHAR OS SPRITES NA ORDEM CORRETA:

    // 1. Dante (silhueta ou revelado)
    if (strcmp(gSelectedCharacterName, "Dante") == 0 && spriteTrocaRealizada)
    {
        DrawTexturePro(carinhaRevelation,
                       (Rectangle){0, 0, carinhaRevelation.width, carinhaRevelation.height},
                       (Rectangle){initialPos, yCarinha,
                                   carinhaRevelation.width * scaleCarinha,
                                   carinhaRevelation.height * scaleCarinha},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        DrawTexturePro(carinhaRevelationSilhouette,
                       (Rectangle){0, 0, carinhaRevelationSilhouette.width, carinhaRevelationSilhouette.height},
                       (Rectangle){initialPos, yCarinha,
                                   carinhaRevelationSilhouette.width * scaleCarinha,
                                   carinhaRevelationSilhouette.height * scaleCarinha},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }

    // 2. Alice (silhueta ou revelada)
    if (strcmp(gSelectedCharacterName, "Alice") == 0 && meninaTrocaRealizada)
    {
        DrawTexturePro(meninaRevelation,
                       (Rectangle){0, 0, meninaRevelation.width, meninaRevelation.height},
                       (Rectangle){xMenina, yMenina,
                                   meninaRevelation.width * scaleMenina,
                                   meninaRevelation.height * scaleMenina},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        DrawTexturePro(meninaRevelationSilhouette,
                       (Rectangle){0, 0, meninaRevelationSilhouette.width, meninaRevelationSilhouette.height},
                       (Rectangle){xMenina, yMenina,
                                   meninaRevelationSilhouette.width * scaleMenina,
                                   meninaRevelationSilhouette.height * scaleMenina},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }

    // 3. Levi (silhueta ou revelado)
    if (strcmp(gSelectedCharacterName, "Levi") == 0 && calvoTrocaRealizada)
    {
        DrawTexturePro(calvoRevelation,
                       (Rectangle){0, 0, calvoRevelation.width, calvoRevelation.height},
                       (Rectangle){xCalvo, yCalvo,
                                   calvoRevelation.width * scaleCalvo,
                                   calvoRevelation.height * scaleCalvo},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        DrawTexturePro(calvoRevelationSilhouette,
                       (Rectangle){0, 0, calvoRevelationSilhouette.width, calvoRevelationSilhouette.height},
                       (Rectangle){xCalvo, yCalvo,
                                   calvoRevelationSilhouette.width * scaleCalvo,
                                   calvoRevelationSilhouette.height * scaleCalvo},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }

    // 4. Jade (silhueta ou revelada)
    if (strcmp(gSelectedCharacterName, "Jade") == 0 && deBoneTrocaRealizada)
    {
        DrawTexturePro(deBoneRevelation,
                       (Rectangle){0, 0, deBoneRevelation.width, deBoneRevelation.height},
                       (Rectangle){xDeBone, yDeBone,
                                   deBoneRevelation.width * scaleDeBone,
                                   deBoneRevelation.height * scaleDeBone},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }
    else
    {
        DrawTexturePro(deBoneRevelationSilhouette,
                       (Rectangle){0, 0, deBoneRevelationSilhouette.width, deBoneRevelationSilhouette.height},
                       (Rectangle){xDeBone, yDeBone,
                                   deBoneRevelationSilhouette.width * scaleDeBone,
                                   deBoneRevelationSilhouette.height * scaleDeBone},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }

    // Efeito de flash
    if (flashAlpha > 0.0f)
    {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                      (Color){255, 255, 255, (unsigned char)(flashAlpha * 255)});
    }

    if (shieldAppeared)
    {
        int shieldX = 50;                                            // Posição X (meio e perto do canto esquerdo)
        int shieldY = screenHeight / 2 - cybertechShield.height / 2; // Posição Y (centralizado verticalmente)

        // Aplica a escala 0.5x ao escudo
        float scale = 0.5f;

        // Desenha o escudo com escala aplicada
        DrawTexturePro(cybertechShield,
                       (Rectangle){0, 0, cybertechShield.width, cybertechShield.height},
                       (Rectangle){shieldX, shieldY,
                                   cybertechShield.width * scale,
                                   cybertechShield.height * scale},
                       (Vector2){0, 0}, 0.0f, WHITE);
    }

    EndDrawing();
}

bool Fase_Tela02_Concluida(void)
{
    return fase_concluida;
}

void Unload_Tela_02(void)
{
    UnloadTexture(carinhaRevelationSilhouette);
    UnloadTexture(carinhaRevelation);
    UnloadTexture(meninaRevelationSilhouette);
    UnloadTexture(meninaRevelation);
    UnloadTexture(calvoRevelationSilhouette);
    UnloadTexture(calvoRevelation);
    UnloadTexture(deBoneRevelationSilhouette);
    UnloadTexture(deBoneRevelation);
    UnloadTexture(cybertechShield);

    StopMusicStream(writtenInTheStars);
    UnloadMusicStream(writtenInTheStars);
    UnloadSound(surpriseSound); // Descarregar o som de surpresa
    UnloadSound(congrats);
}