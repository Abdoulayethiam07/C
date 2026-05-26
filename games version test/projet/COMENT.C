#include <stdio.h>
#include <raylib.h>
#include <raymath.h>

int main(void)
{
    int playerSpeed = 4;
    Vector2 player = {400, 225};
    InitWindow(800, 450, "raylib [core] example - basic window");

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(RAYWHITE);
        if (IsKeyDown(KEY_RIGHT))
            player.x += playerSpeed;
        if (IsKeyDown(KEY_LEFT))
            player.x -= playerSpeed;

        DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
