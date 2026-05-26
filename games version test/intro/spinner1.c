#include <stdio.h>
#include <raylib.h>

#define SCREENWIDTH 800
#define SCREENHEIGHT 600
#define FPS 60

int main(void)
{
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Spinner");

    // Charger UNE seule fois
    Texture2D spinner = LoadTexture("image/spinner.jpg");

    float rotation = 0.0f;

    SetTargetFPS(120);

    while (!WindowShouldClose())
    {
        rotation += 5.0f; // animation

        Rectangle source = {0.0f, 0.0f, (float)spinner.width, (float)spinner.height};
        Rectangle dest = {400, 300, 200, 200};
        Vector2 origin = {0, 0};
        if (IsKeyDown(KEY_RIGHT))
            rotation += 2;
        if (IsKeyDown(KEY_LEFT))
            rotation -= 2;

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawTexturePro(
            spinner,
            source,
            dest,
            origin,
            rotation,
            RAYWHITE);

        DrawText("Rotation autour du centre (Pivot)", 10, 10, 20, DARKGRAY);

        EndDrawing();
    }

    UnloadTexture(spinner);
    CloseWindow();

    return 0;
}