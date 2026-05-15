// ===============================
//      BASE SIMPLE JEU RAYLIB
// ===============================

// bibliothèque principale raylib
#include "raylib.h"

// ---------- constantes ----------

// largeur fenêtre
#define SCREEN_WIDTH 700

// hauteur fenêtre
#define SCREEN_HEIGHT 650

// FPS
#define FPS 60

// ---------- fonction principale ----------

int main(void)
{

    // ==================================
    //      INITIALISATION
    // ==================================

    // création de la fenêtre
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Jeu voiture");

    // fixe les FPS
    SetTargetFPS(FPS);

    // ==================================
    //      JOUEUR
    // ==================================

    // position du joueur
    Vector2 playerPosition = {330, 500};

    // vitesse du joueur
    int playerSpeed = 5;

    // rectangle joueur
    Rectangle player;

    // ==================================
    //      BOUCLE PRINCIPALE
    // ==================================

    // le jeu tourne tant que la fenêtre est ouverte
    while (!WindowShouldClose())
    {

        // ==================================
        //      UPDATE
        // ==================================

        // mise à jour du rectangle joueur
        player = (Rectangle){
            playerPosition.x, // position X
            playerPosition.y, // position Y
            50,               // largeur
            80                // hauteur
        };

        // ==================================
        //      MOUVEMENT JOUEUR
        // ==================================

        // déplacement gauche
        if (IsKeyDown(KEY_LEFT))
        {
            playerPosition.x -= playerSpeed;
        }

        // déplacement droite
        if (IsKeyDown(KEY_RIGHT))
        {
            playerPosition.x += playerSpeed;
        }

        // déplacement haut
        if (IsKeyDown(KEY_UP))
        {
            playerPosition.y -= playerSpeed;
        }

        // déplacement bas
        if (IsKeyDown(KEY_DOWN))
        {
            playerPosition.y += playerSpeed;
        }

        // ==================================
        //      LIMITES ECRAN
        // ==================================

        // empêche sortie gauche
        if (playerPosition.x < 0)
        {
            playerPosition.x = 0;
        }

        // empêche sortie droite
        if (playerPosition.x > SCREEN_WIDTH - 50)
        {
            playerPosition.x = SCREEN_WIDTH - 50;
        }

        // empêche sortie haut
        if (playerPosition.y < 0)
        {
            playerPosition.y = 0;
        }

        // empêche sortie bas
        if (playerPosition.y > SCREEN_HEIGHT - 80)
        {
            playerPosition.y = SCREEN_HEIGHT - 80;
        }

        // ==================================
        //      DESSIN
        // ==================================

        BeginDrawing();

        // couleur fond
        ClearBackground(DARKGRAY);

        // dessin route
        DrawRectangle(100, 0, 500, SCREEN_HEIGHT, GRAY);

        // dessin joueur
        DrawRectangleRec(player, BLUE);

        EndDrawing();
    }

    // ==================================
    //      FERMETURE
    // ==================================

    // ferme fenêtre
    CloseWindow();

    return 0;
}