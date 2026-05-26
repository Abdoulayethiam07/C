#include "raylib.h"

int main(void){
    // Initialisation
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Mon premier program Raylib");
    
    // Boucle principale
    while (!WindowShouldClose()) { // Vérifie si ESC ou croix est pressé
        // Logique du jeu (mise à jour)
        
        // Dessin
        BeginDrawing();
            ClearBackground(RAYWHITE);  // Efface l'écran
            DrawText("Bonjour 1A!", 190, 200, 20, BLACK);
        EndDrawing();
    }
    
    // Nettoyage
    CloseWindow();
    
    return 0;
}