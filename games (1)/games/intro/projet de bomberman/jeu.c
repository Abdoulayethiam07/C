#include <stdio.h>
#include "raylib.h"
#define SCREEN_HEIGHT 640
#define SCREEN_WIDHT 640
#define FPS 60

// Espace Fonction et structure de donnée

int main(void)
{

    InitWindow(SCREEN_WIDHT, SCREEN_HEIGHT, "Bomberman");

    Texture2D image_cadre = LoadTexture("image_niveau_1/block_12.jpeg");
    Texture2D image_destructible = LoadTexture("image_niveau_1/block_11.jpeg");
    Texture2D image_N_destructible = LoadTexture("image_niveau_1/block_10.jpeg");
    Rectangle source_cadre = {0, 0, image_cadre.width, image_cadre.height};
    Rectangle source_destructible = {0, 0, image_destructible.width, image_destructible.height};
    Rectangle source_N_destructible = {0, 0, image_N_destructible.width, image_N_destructible.height};
    Rectangle dest_cadre_GD[16][2];
    Rectangle dest_cadre_HB[2][15];
    int grille_de_jeu[13][14] = {
        {0, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0},
        {0, 2, 2, 2, 1, 2, 1, 2, 1, 2, 2, 2, 2, 0},
        {1, 2, 1, 1, 1, 1, 0, 0, 1, 1, 1, 2, 1, 1},
        {1, 2, 0, 2, 2, 2, 2, 2, 2, 2, 1, 0, 2, 1},
        {1, 1, 1, 1, 0, 2, 0, 0, 1, 1, 1, 1, 1, 0},
        {0, 2, 1, 2, 1, 2, 0, 2, 1, 2, 1, 2, 2, 1},
        {1, 1, 1, 1, 0, 0, 0, 2, 1, 1, 1, 2, 1, 0},
        {1, 2, 1, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 1},
        {1, 2, 1, 1, 0, 1, 1, 1, 1, 0, 1, 2, 1, 0},
        {0, 2, 2, 2, 0, 2, 1, 2, 1, 2, 2, 2, 2, 1},
        {1, 1, 0, 1, 0, 2, 0, 2, 1, 0, 1, 1, 1, 1},
        {0, 2, 1, 2, 0, 1, 0, 2, 2, 1, 2, 1, 2, 0},
        {0, 0, 1, 2, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0}};
    int a;
    int i;
    int Taille_block = 40;
    Vector2 origin = {0, 0};

    for (a = 0; a < 16; a++)
    {
        dest_cadre_GD[a][0] = (Rectangle){0, Taille_block, 40, 40};
        dest_cadre_GD[a][1] = (Rectangle){600, Taille_block, 40, 40};
        Taille_block += 40;
    }
    Taille_block = 0;
    for (a = 0; a < 15; a++)
    {
        dest_cadre_HB[0][a] = (Rectangle){Taille_block, 40, 40, 40};
        dest_cadre_HB[1][a] = (Rectangle){Taille_block, 600, 40, 40};
        Taille_block += 40;
    }

    while (!WindowShouldClose())
    {

        BeginDrawing();
        ClearBackground(GREEN);
        DrawLine(0, 40, 650, 40, WHITE);
        DrawText("Espace reservé au score et autre", 50, 10, 30, WHITE);
        for (a = 0; a < 16; a++)
        {
            DrawTexturePro(image_cadre, source_cadre, dest_cadre_GD[a][0], origin, 0, WHITE);
            DrawTexturePro(image_cadre, source_cadre, dest_cadre_GD[a][1], origin, 0, WHITE);
        }
        for (a = 0; a < 15; a++)
        {
            DrawTexturePro(image_cadre, source_cadre, dest_cadre_HB[0][a], origin, 0, WHITE);
            DrawTexturePro(image_cadre, source_cadre, dest_cadre_HB[1][a], origin, 0, WHITE);
        }

        for (a = 0; a < 13; a++)
        {
            for (i = 0; i < 14; i++)
            {
                if (grille_de_jeu[a][i] == 1)
                {
                    DrawTexturePro(image_destructible, source_destructible, (Rectangle){40 + (40 * i), (40 * a) + 80, 40, 40}, origin, 0, WHITE);
                }
                if (grille_de_jeu[a][i] == 2)
                {
                    DrawTexturePro(image_cadre, source_cadre, (Rectangle){40 + (40 * i), (40 * a) + 80, 40, 40}, origin, 0, WHITE);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
}
