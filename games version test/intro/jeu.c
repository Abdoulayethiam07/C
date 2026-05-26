#include<stdio.h>
#include "raylib.h"

// --- CONSTANTES ---
#define TILE_SIZE 40
#define MAP_WIDTH 15
#define MAP_HEIGHT 13
#define SCREEN_WIDTH (MAP_WIDTH * TILE_SIZE)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_SIZE + 60) // +60 pixels pour le bandeau du haut (Score/Temps)

// --- ENUMS ET STRUCTURES ---
typedef enum GameScreen
{
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_WIN
} GameScreen;

typedef struct Player
{
    Vector2 position;
    float speed;
    int currentLevel;
} Player;

// --- CARTES DES NIVEAUX (0: Vide, 1: Incassable, 2: Tonneau/Caisse) ---
int level1[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 1},
    {1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 1},
    {1, 0, 1, 2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 0, 1},
    {1, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 1},
    {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1},
    {1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 1},
    {1, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1},
    {1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
    {1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1},
    {1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 1},
    {1, 2, 1, 2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 2, 1},
    {1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 1},
    {1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1},
    {1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1},
    {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

// Pointeur pour charger le niveau actif
int (*currentMap)[MAP_WIDTH];

// --- FONCTION DE COLLISION AVEC LA GRILLE ---
bool CheckWallCollision(Vector2 nextPos)
{
    // On vérifie les 4 coins du joueur pour éviter qu'il morde sur un mur
    int margin = 4;
    int leftTile = (nextPos.x + margin) / TILE_SIZE;
    int rightTile = (nextPos.x + TILE_SIZE - margin) / TILE_SIZE;
    int topTile = (nextPos.y + margin) / TILE_SIZE;
    int bottomTile = (nextPos.y + TILE_SIZE - margin) / TILE_SIZE;

    // Si l'un des coins touche un mur (1) ou un tonneau (2)
    if (currentMap[topTile][leftTile] != 0 || currentMap[topTile][rightTile] != 0 ||
        currentMap[bottomTile][leftTile] != 0 || currentMap[bottomTile][rightTile] != 0)
    {
        return true;
    }
    return false;
}

// --- PROGRAMME PRINCIPAL ---
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bomberman Prototyping");
    SetTargetFPS(60);

    GameScreen currentScreen = SCREEN_MENU;

    // Initialisation du joueur
    Player player = {0};
    player.position = (Vector2){TILE_SIZE, TILE_SIZE * 1 + 60}; // Aligné sous le bandeau du haut
    player.speed = 3.0f;
    player.currentLevel = 1;
    currentMap = level1;

    while (!WindowShouldClose())
    {
        // ---------------------------------------------------------------------
        // 1. MISE À JOUR DE LA LOGIQUE (UPDATE)
        // ---------------------------------------------------------------------
        switch (currentScreen)
        {
        case SCREEN_MENU:
        {
            // Si on appuie sur ENTRER, on bascule sur le jeu
            if (IsKeyPressed(KEY_ENTER))
            {
                currentScreen = SCREEN_GAMEPLAY;
            }
        }
        break;

        case SCREEN_GAMEPLAY:
        {
            Vector2 nextPosition = player.position;

            // Gestion des déplacements au clavier
            if (IsKeyDown(KEY_RIGHT))
                nextPosition.x += player.speed;
            if (IsKeyDown(KEY_LEFT))
                nextPosition.x -= player.speed;
            if (IsKeyDown(KEY_DOWN))
                nextPosition.y += player.speed;
            if (IsKeyDown(KEY_UP))
                nextPosition.y -= player.speed;

            // Application du déplacement si pas de collision
            if (!CheckWallCollision((Vector2){nextPosition.x, player.position.y}))
            {
                player.position.x = nextPosition.x;
            }
            if (!CheckWallCollision((Vector2){player.position.x, nextPosition.y}))
            {
                player.position.y = nextPosition.y;
            }

            // Touche "N" temporaire pour simuler le passage au Niveau 2
            if (IsKeyPressed(KEY_N) && player.currentLevel == 1)
            {
                player.currentLevel = 2;
                currentMap = level2;
                player.position = (Vector2){TILE_SIZE, TILE_SIZE * 1 + 60}; // Reset position
            }
            // Touche "G" temporaire pour simuler la victoire
            if (IsKeyPressed(KEY_G))
            {
                currentScreen = SCREEN_WIN;
            }
        }
        break;

        case SCREEN_WIN:
        {
            if (IsKeyPressed(KEY_ENTER))
            {
                // Reset pour rejouer
                player.currentLevel = 1;
                currentMap = level1;
                player.position = (Vector2){TILE_SIZE, TILE_SIZE * 1 + 60};
                currentScreen = SCREEN_MENU;
            }
        }
        break;
        }

        // ---------------------------------------------------------------------
        // 2. DESSIN DU JEU (DRAW)
        // ---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(RAYWHITE);

        switch (currentScreen)
        {
        case SCREEN_MENU:
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
            DrawText("BOMBERMAN", SCREEN_WIDTH / 2 - MeasureText("BOMBERMAN", 40) / 2, SCREEN_HEIGHT / 3, 40, YELLOW);
            DrawText("Appuyez sur [ENTRER] pour Jouer", SCREEN_WIDTH / 2 - MeasureText("Appuyez sur [ENTRER] pour Jouer", 20) / 2, SCREEN_HEIGHT / 2, 20, WHITE);
        }
        break;

        case SCREEN_GAMEPLAY:
        {
            // Dessin du bandeau supérieur (Interface utilisateur comme dans la vidéo)
            DrawRectangle(0, 0, SCREEN_WIDTH, 60, BLACK);
            DrawText(TextFormat("NIVEAU: %d", player.currentLevel), 20, 20, 20, GOLD);
            DrawText("PROJET C / RAYLIB", SCREEN_WIDTH - 220, 20, 20, LIGHTGRAY);
            DrawText("Touche [N]: Niv. Suivant | [G]: Gagner", 200, 23, 14, WHITE);

            // Dessin de la grille de jeu
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    int tileType = currentMap[y][x];
                    int posY = y * TILE_SIZE + 60; // Décalage à cause du bandeau
                    int posX = x * TILE_SIZE;

                    if (tileType == 1)
                    {
                        // Mur incassable (Gris foncé)
                        DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, DARKGRAY);
                        DrawRectangleLines(posX, posY, TILE_SIZE, TILE_SIZE, BLACK);
                    }
                    else if (tileType == 2)
                    {
                        // Tonneau/Obstacle destructible (Marron)
                        DrawRectangle(posX + 2, posY + 2, TILE_SIZE - 4, TILE_SIZE - 4, BROWN);
                        DrawRectangleLines(posX + 2, posY + 2, TILE_SIZE - 4, TILE_SIZE - 4, DARKBROWN);
                    }
                    else
                    {
                        // Sol (Gris clair pour l'effet dalles)
                        DrawRectangle(posX, posY, TILE_SIZE, TILE_SIZE, LIGHTGRAY);
                        DrawRectangleLines(posX, posY, TILE_SIZE, TILE_SIZE, GRAY);
                    }
                }
            }

            // Dessin du joueur (Un rond ou un rectangle bleu en attendant vos textures)
            DrawRectangle(player.position.x + 4, player.position.y + 4, TILE_SIZE - 8, TILE_SIZE - 8, BLUE);
        }
        break;

        case SCREEN_WIN:
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
            DrawText("VICTOIRE !", SCREEN_WIDTH / 2 - MeasureText("VICTOIRE !", 40) / 2, SCREEN_HEIGHT / 3, 40, WHITE);
            DrawText("Appuyez sur [ENTRER] pour revenir au menu", SCREEN_WIDTH / 2 - MeasureText("Appuyez sur [ENTRER] pour revenir au menu", 20) / 2, SCREEN_HEIGHT / 2, 20, DARKGREEN);
        }
        break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}