#include <stdio.h>
#include "raylib.h"
#include "math.h"

// --- Partie de Junior & Abdoulaye (Constantes partagées) ---
#define TILE_SIZE 40
#define MAP_WIDTH 15
#define MAP_HEIGHT 14P_WIDTH * TILE_SIZE)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_SIZE + 60)
#define MAX_BOMBS 3
#define BOMB_FUSE_TIME 2.5f
#define EXPLOSION_TIME 0.5f
#define BOMB_RADIUS 2
#define MAX_EXPLOSIONS 50
#define FPS 60

// --- Partie de Junior ---
typedef enum GameScreen
{
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_WIN
} GameScreen;

// --- Partie de Abdoulaye ---
typedef enum Direction
{
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP
} Direction;

typedef struct Player
{
    Vector2 position;
    float speed;
    int currentLevel;
    Direction dir;
    bool isMoving;
    int frame;
    float animationTimer;
} Player;

typedef struct Bomb
{
    int tileX;
    int tileY;
    float timer;
    bool active;
} Bomb;

typedef struct Explosion
{
    int tileX;
    int tileY;
    float timer;
    bool active;
} Explosion;

// --- Partie de Junior (Cartographie d'origine) ---
int level1[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 1}, {1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 1}, {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1}, {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}, {1, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 1}, {1, 0, 1, 2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 0, 1}, {1, 0, 2, 2, 2, 0, 0, 0, 0, 0, 2, 2, 2, 0, 1}, {1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1}, {1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1}, {1, 0, 1, 2, 1, 2, 1, 2, 1, 2, 1, 2, 1, 0, 1}, {1, 0, 0, 2, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1}, {1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1}, {1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1}, {1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 1}, {1, 2, 1, 2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 2, 1}, {1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 1}, {1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1}, {1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1}, {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

int (*currentMap)[MAP_WIDTH];
Bomb bombs[MAX_BOMBS] = {0};
Explosion explosions[MAX_EXPLOSIONS] = {0};

// --- Partie de Abdoulaye (Gestion des Bombes) ---
bool IsBombAtTile(int x, int y)
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].active && bombs[i].tileX == x && bombs[i].tileY == y)
            return true;
    }
    return false;
}

// --- Amélioration par Abdoulaye sur la base des Collisions de Junior ---
bool CheckWallCollision(Vector2 nextPos, Player p)
{
    int m = 6; // Marge de Junior pour glisser sur les coins
    int l = (nextPos.x + m) / TILE_SIZE, r = (nextPos.x + TILE_SIZE - m) / TILE_SIZE;
    int t = (nextPos.y - 60 + m) / TILE_SIZE, b = (nextPos.y - 60 + TILE_SIZE - m) / TILE_SIZE;

    if (l < 0 || r >= MAP_WIDTH || t < 0 || b >= MAP_HEIGHT)
        return true;
    if (currentMap[t][l] || currentMap[t][r] || currentMap[b][l] || currentMap[b][r])
        return true;

    // Blocage sur les bombes ajouté par Abdoulaye
    int px = (p.position.x + TILE_SIZE / 2) / TILE_SIZE;
    int py = (p.position.y - 60 + TILE_SIZE / 2) / TILE_SIZE;

    if (IsBombAtTile(l, t) && (l != px || t != py))
        return true;
    if (IsBombAtTile(r, t) && (r != px || t != py))
        return true;
    if (IsBombAtTile(l, b) && (l != px || b != py))
        return true;
    if (IsBombAtTile(r, b) && (r != px || b != py))
        return true;
    return false;
}

// --- Partie de Abdoulaye (Système d'explosions) ---
void AddExplosion(int x, int y)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (!explosions[i].active)
        {
            explosions[i] = (Explosion){x, y, EXPLOSION_TIME, true};
            break;
        }
    }
}

void TriggerExplosion(int cx, int cy)
{
    AddExplosion(cx, cy);
    int dx[] = {0, 0, -1, 1}, dy[] = {-1, 1, 0, 0};
    for (int i = 0; i < 4; i++)
    {
        for (int r = 1; r <= BOMB_RADIUS; r++)
        {
            int tx = cx + dx[i] * r, ty = cy + dy[i] * r;
            if (tx < 0 || tx >= MAP_WIDTH || ty < 0 || ty >= MAP_HEIGHT)
                break;
            if (currentMap[ty][tx] == 1)
                break;
            if (currentMap[ty][tx] == 2)
            {
                currentMap[ty][tx] = 0;
                AddExplosion(tx, ty);
                break;
            }
            if (currentMap[ty][tx] == 0)
                AddExplosion(tx, ty);
        }
    }
}

// --- PROGRAMME PRINCIPAL ---
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bomberman - Duo Project");
    SetTargetFPS(FPS);

    // --- Partie de Abdoulaye (Chargement des Assets Graphiques) ---
    Texture2D texPlayer = LoadTexture("../intro/images/bonhomme.png");
    float frameWidth = (float)texPlayer.width / 3;
    float frameHeight = (float)texPlayer.height / 4;
    Rectangle sourceRec = {0.0f, 0.0f, frameWidth, frameHeight};

    Texture2D texFloor = LoadTexture("../intro/images/herbe.png");
    Texture2D texWall = LoadTexture("../intro/images/murgris.png");
    Texture2D texCrate = LoadTexture("../intro/images/tonneaux.png");

    Texture2D texBomb = {0};
    Texture2D texFire = {0};
    if (FileExists("../intro/images/bomb.png"))
        texBomb = LoadTexture("../intro/images/bomb.png");
    if (FileExists("../intro/images/fire.png"))
        texFire = LoadTexture("../intro/images/fire.png");

    int currentFrame = 0;
    float frameCounter = 0.0f;
    float frameSpeed = 12.0f;

    // --- Partie de Junior (Initialisation des variables de gameplay et de la carte) ---
    GameScreen currentScreen = SCREEN_MENU;
    Player player = {(Vector2){TILE_SIZE, TILE_SIZE + 60}, 3.0f, 1, DIR_DOWN, false, 1, 0.0f};
    currentMap = level1;

    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // ---------------------------------------------------------------------
        // 1. MISE À JOUR DE LA LOGIQUE (UPDATE)
        // ---------------------------------------------------------------------
        switch (currentScreen)
        {
        case SCREEN_MENU:
            // --- Partie de Junior ---
            if (IsKeyPressed(KEY_ENTER))
                currentScreen = SCREEN_GAMEPLAY;
            break;

        case SCREEN_GAMEPLAY:
        {
            Vector2 nextPos = player.position;
            player.isMoving = false;

            // --- Partie de Junior (Mouvements claviers d'origine) ---
            if (IsKeyDown(KEY_RIGHT))
            {
                nextPos.x += player.speed;
                player.dir = DIR_RIGHT;
                player.isMoving = true;
            }
            else if (IsKeyDown(KEY_LEFT))
            {
                nextPos.x -= player.speed;
                player.dir = DIR_LEFT;
                player.isMoving = true;
            }
            else if (IsKeyDown(KEY_DOWN))
            {
                nextPos.y += player.speed;
                player.dir = DIR_DOWN;
                player.isMoving = true;
            }
            else if (IsKeyDown(KEY_UP))
            {
                nextPos.y -= player.speed;
                player.dir = DIR_UP;
                player.isMoving = true;
            }

            // --- Partie de Junior (Physique de déplacement appliquée) ---
            if (!CheckWallCollision((Vector2){nextPos.x, player.position.y}, player))
                player.position.x = nextPos.x;
            if (!CheckWallCollision((Vector2){player.position.x, nextPos.y}, player))
                player.position.y = nextPos.y;

            // --- Partie de Abdoulaye (Calcul de l'animation du Sprite) ---
            if (player.isMoving)
            {
                frameCounter += dt;
                if (frameCounter >= (1.0f / frameSpeed))
                {
                    frameCounter = 0.0f;
                    currentFrame = (currentFrame + 1) % 3;
                }
            }
            else
            {
                currentFrame = 1;
            }
            sourceRec.x = currentFrame * frameWidth;
            sourceRec.y = player.dir * frameHeight;

            // --- Partie de Abdoulaye (Contrôles espace pour poser la bombe) ---
            if (IsKeyPressed(KEY_SPACE))
            {
                int tx = (player.position.x + TILE_SIZE / 2) / TILE_SIZE;
                int ty = (player.position.y - 60 + TILE_SIZE / 2) / TILE_SIZE;
                if (!IsBombAtTile(tx, ty))
                {
                    for (int i = 0; i < MAX_BOMBS; i++)
                    {
                        if (!bombs[i].active)
                        {
                            bombs[i] = (Bomb){tx, ty, BOMB_FUSE_TIME, true};
                            break;
                        }
                    }
                }
            }

            // --- Partie de Abdoulaye (Mise à jour des timers d'explosions) ---
            for (int i = 0; i < MAX_BOMBS; i++)
            {
                if (bombs[i].active)
                {
                    bombs[i].timer -= dt;
                    if (bombs[i].timer <= 0.0f)
                    {
                        bombs[i].active = false;
                        TriggerExplosion(bombs[i].tileX, bombs[i].tileY);
                    }
                }
            }
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosions[i].active)
                {
                    explosions[i].timer -= dt;
                    if (explosions[i].timer <= 0.0f)
                        explosions[i].active = false;
                }
            }

            // --- Partie de Junior (Système de triche et changement de carte) ---
            if (IsKeyPressed(KEY_N) && player.currentLevel == 1)
            {
                player.currentLevel = 2;
                currentMap = level2;
                player.position = (Vector2){TILE_SIZE, TILE_SIZE + 60};
            }
            if (IsKeyPressed(KEY_G))
                currentScreen = SCREEN_WIN;
        }
        break;

        case SCREEN_WIN:
            // --- Partie de Junior ---
            if (IsKeyPressed(KEY_ENTER))
            {
                player.currentLevel = 1;
                currentMap = level1;
                player.position = (Vector2){TILE_SIZE, TILE_SIZE + 60};
                player.dir = DIR_DOWN;
                currentScreen = SCREEN_MENU;
            }
            break;
        }

        // ---------------------------------------------------------------------
        // 2. DESSIN DU JEU (DRAW)
        // ---------------------------------------------------------------------
        BeginDrawing();
        ClearBackground(BLUE);

        if (currentScreen == SCREEN_MENU)
        {
            // --- Partie de Junior (Design du Menu) ---
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
            DrawText("BOMBERMAN", SCREEN_WIDTH / 2 - MeasureText("BOMBERMAN", 40) / 2, SCREEN_HEIGHT / 3, 40, YELLOW);
            DrawText("Appuyez sur [ENTRER] pour Jouer", SCREEN_WIDTH / 2 - MeasureText("Appuyez sur [ENTRER] pour Jouer", 20) / 2, SCREEN_HEIGHT / 2, 20, RED);
        }
        else if (currentScreen == SCREEN_GAMEPLAY)
        {
            // --- Partie de Junior (Interface du bandeau supérieur) ---
            DrawRectangle(0, 0, SCREEN_WIDTH, 60, BLACK);
            DrawText(TextFormat("NIVEAU: %d", player.currentLevel), 20, 20, 20, GOLD);
            DrawText("PROJET C / RAYLIB", SCREEN_WIDTH - 220, 20, 20, BLUE);
            DrawText("Touche [N](triche): Niv. Suivant | [G]: Gagner", 200, 23, 14, WHITE);

            // --- Rendu visuel (Grille de Junior + Textures de remplacement de Abdoulaye) ---
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    float px = x * TILE_SIZE, py = y * TILE_SIZE + 60;

                    if (texFloor.width > 0)
                        DrawTexturePro(texFloor, (Rectangle){0, 0, texFloor.width, texFloor.height}, (Rectangle){px, py, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                    else
                        DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, LIGHTGRAY);

                    if (currentMap[y][x] == 1)
                    {
                        if (texWall.width > 0)
                            DrawTexturePro(texWall, (Rectangle){0, 0, texWall.width, texWall.height}, (Rectangle){px, py, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                        else
                        {
                            DrawRectangle(px, py, TILE_SIZE, TILE_SIZE, DARKGRAY);
                            DrawRectangleLines(px, py, TILE_SIZE, TILE_SIZE, BLACK);
                        }
                    }
                    if (currentMap[y][x] == 2)
                    {
                        if (texCrate.width > 0)
                            DrawTexturePro(texCrate, (Rectangle){0, 0, texCrate.width, texCrate.height}, (Rectangle){px, py, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                        else
                        {
                            DrawRectangle(px + 2, py + 2, TILE_SIZE - 4, TILE_SIZE - 4, BROWN);
                            DrawRectangleLines(px + 2, py + 2, TILE_SIZE - 4, TILE_SIZE - 4, DARKBROWN);
                        }
                    }
                }
            }

            // --- Partie de Abdoulaye (Affichage des Bombes) ---
            for (int i = 0; i < MAX_BOMBS; i++)
            {
                if (bombs[i].active)
                {
                    float p = 1.0f + (sinf(bombs[i].timer * 10.0f) * 0.05f);
                    float bx = bombs[i].tileX * TILE_SIZE + (TILE_SIZE - TILE_SIZE * p) / 2;
                    float by = bombs[i].tileY * TILE_SIZE + 60 + (TILE_SIZE - TILE_SIZE * p) / 2;
                    if (texBomb.width > 0)
                        DrawTexturePro(texBomb, (Rectangle){0, 0, texBomb.width, texBomb.height}, (Rectangle){bx, by, TILE_SIZE * p, TILE_SIZE * p}, (Vector2){0, 0}, 0.0f, WHITE);
                    else
                        DrawCircleV((Vector2){bx + TILE_SIZE * p / 2, by + TILE_SIZE * p / 2}, (TILE_SIZE * p) * 0.35f, BLACK);
                }
            }

            // --- Partie de Abdoulaye (Affichage des Explosions) ---
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosions[i].active)
                {
                    if (texFire.width > 0)
                        DrawTexturePro(texFire, (Rectangle){0, 0, texFire.width, texFire.height}, (Rectangle){explosions[i].tileX * TILE_SIZE, explosions[i].tileY * TILE_SIZE + 60, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                    else
                        DrawRectangle(explosions[i].tileX * TILE_SIZE, explosions[i].tileY * TILE_SIZE + 60, TILE_SIZE, TILE_SIZE, ORANGE);
                }
            }

            // --- Affichage Player (Sprite d'Abdoulaye ou Carré de secours de Junior) ---
            if (texPlayer.width > 0)
            {
                DrawTexturePro(texPlayer, sourceRec, (Rectangle){player.position.x, player.position.y, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
            }
            else
            {
                DrawRectangle(player.position.x + 4, player.position.y + 4, TILE_SIZE - 8, TILE_SIZE - 8, BLUE);
            }
        }
        else if (currentScreen == SCREEN_WIN)
        {
            // --- Partie de Junior (Écran de Fin) ---
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
            DrawText("VICTOIRE !", SCREEN_WIDTH / 2 - MeasureText("VICTOIRE !", 40) / 2, SCREEN_HEIGHT / 3, 40, WHITE);
            DrawText("Appuyez sur [ENTRER] pour revenir au menu", SCREEN_WIDTH / 2 - MeasureText("Appuyez sur [ENTRER] pour revenir au menu", 20) / 2, SCREEN_HEIGHT / 2, 20, DARKGREEN);
        }

        EndDrawing();
    }

    // --- Partie de Abdoulaye (Déchargement textures) ---
    UnloadTexture(texFloor);
    UnloadTexture(texWall);
    UnloadTexture(texCrate);
    UnloadTexture(texPlayer);
    if (texBomb.width > 0)
        UnloadTexture(texBomb);
    if (texFire.width > 0)
        UnloadTexture(texFire);

    CloseWindow();
    return 0;
}