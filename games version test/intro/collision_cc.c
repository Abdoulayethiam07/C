#include <stdio.h>
#include<raylib.h>
#define SCREENWIDTH 800
#define SCREENHEIGHT 600
#define FPS 60
int main(void)
{
    // Cercle 1 (contrôlé par souris)

    Vector2 cercle1 = {400, 300};

    int rayon1 = 40;

    // Cercle 2 (fixe)

    Vector2 cercle2 = {600, 300};

    int rayon2 = 50;
    bool collision = false;
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "Collision entre deux cercles");
    SetTargetFPS(FPS);
    while (!WindowShouldClose()){
        cercle1.x = GetMouseX();

        cercle1.y = GetMouseY();
        collision = CheckCollisionCircles(cercle1, rayon1, cercle2, rayon2);
        Color couleur1 = collision ? RED : BLUE;

        Color couleur2 = collision ? ORANGE : GREEN;
        BeginDrawing();
        ClearBackground(RAYWHITE);
            DrawCircle(cercle1.x, cercle1.y, rayon1, couleur1);

            DrawCircleLines(cercle1.x, cercle1.y, rayon1, BLACK);

            DrawCircle(cercle2.x, cercle2.y, rayon2, couleur2);

            DrawCircleLines(cercle2.x, cercle2.y, rayon2, BLACK);
            DrawText("Deplacez le cercle bleu avec la souris", 200, 50, 20, DARKGRAY);
            if (collision)
            {

                DrawText("COLLISION !", 350, 200, 30, RED);
            }
            else
            {
                
                DrawText("Pas de collision", 330, 200, 30, DARKGRAY);
            }
        EndDrawing();  

    }
    CloseWindow();

}