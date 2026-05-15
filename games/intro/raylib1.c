#include "raylib.h"

int main(void){
    // Initialisation
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "Mon premier program Raylib");
    
    // Boucle principale
    while (!WindowShouldClose()) { // Vérifie si ESC ou croix et pressé
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

// --- Build configuration ---
// Tasks
// build
// {
//     "label": "build",
//     "type": "shell",
//     "command": "gcc",
//     "args": [
//         "raylib1.c",
//         "-o",
//         "raylib1.exe",
//         "-I",
//         "C:\\raylib\\include",
//         "-L",
//         "C:\\raylib\\lib",
//         "-lraylib",
//         "-lopengl32",
//         "-lgdi32",
//         "-lwinmm"
//     ],
//     "group": {
//         "kind": "build",
//         "isDefault": true
//     },
//     "presentation": {
//         "echo": true,
//         "reveal": "silent",
//         "focus": false,
//         "panel": "shared"
//     },
//     "problemMatcher": "$gcc"
// }
// --- End Build configuration ---