#include <stdio.h>
#include <raylib.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600
#define FPS 12

int main(void)
{
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Animation Sprite");

    // Charger UNE seule fois
    Texture2D sprite = LoadTexture("images/sprite.jpg");

    float frameWidth = (float)sprite.width / 3;
    float frameHeight = (float)sprite.height / 4;

    Rectangle sourceRec = {0.0f, 0.0f, frameWidth, frameHeight};
    Rectangle destRec = {400, 300, 200, 200};

    Vector2 origin = {0, 0};

    int currentFrame = 0;
    int totalFrames = 12;

    float frameCounter = 0.0f;
    float frameSpeed = 12.0f;

    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        // Temps écoulé
        frameCounter += GetFrameTime();

        // Changer de frame
        if (frameCounter >= (1.0f / frameSpeed))
        {
            frameCounter = 0.0f;

            currentFrame++;

            // Revenir au début
            if (currentFrame >= totalFrames)
            {
                currentFrame = 0;
            }

            // Calcul colonne et ligne
            int column = currentFrame % 3;
            int row = currentFrame / 3;

            sourceRec.x = column * frameWidth;
            sourceRec.y = row * frameHeight;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawTexturePro(
            sprite,
            sourceRec,
            destRec,
            origin,
            0.0f,
            RAYWHITE);

        DrawText(
            TextFormat(
                "Frame: %i (Col: %i, Lig: %i)",
                currentFrame,
                currentFrame % 3,
                currentFrame / 3),
            10,
            10,
            20,
            DARKGRAY);

        EndDrawing();
    }

    UnloadTexture(sprite);

    CloseWindow();

    return 0;
}