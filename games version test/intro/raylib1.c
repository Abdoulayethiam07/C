#include <stdio.h>
#include "raylib.h"
#include "math.h"

#define TILE_SIZE 40
#define MAP_WIDTH 15
#define MAP_HEIGHT 13
#define SCREEN_WIDTH (MAP_WIDTH * TILE_SIZE)
#define SCREEN_HEIGHT (MAP_HEIGHT * TILE_SIZE + 60)
#define MAX_BOMBS 3
#define BOMB_FUSE_TIME 2.5f
#define EXPLOSION_TIME 0.5f
#define BOMB_RADIUS 2
#define MAX_EXPLOSIONS 50
#define FPS 60

typedef enum
{
    SCREEN_MENU,
    SCREEN_GAMEPLAY,
    SCREEN_WIN
} GameScreen;

typedef enum
{
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_UP
} Direction;

typedef struct
{
    Vector2 position;
    float speed;
    int currentLevel;
    Direction dir;
    bool isMoving;
    int frame;
    
} Player;

typedef struct
{
    int tileX;
    int tileY;
    float timer;
    bool active;
} Bomb;

typedef struct
{
    int tileX;
    int tileY;
    float timer;
    bool active;
} Explosion;

// Cartes du jeu
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
    // 0: Vide, 1: Incassable, 2: Tonneau/Caisse


int level2[MAP_HEIGHT][MAP_WIDTH] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1}, {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1}, {1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1}, {1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1}, {1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 1}, {1, 2, 1, 2, 1, 2, 1, 0, 1, 2, 1, 2, 1, 2, 1}, {1, 2, 2, 2, 2, 2, 2, 0, 2, 2, 2, 2, 2, 2, 1}, {1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1, 2, 1, 1, 1}, {1, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 1}, {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1}, {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};
// 0: Vide, 1: Incassable, 2: Tonneau/Caisse
int (*currentMap)[MAP_WIDTH];
Bomb bombs[MAX_BOMBS] = {0};
Explosion explosions[MAX_EXPLOSIONS] = {0};
// Prototypes des fonctions

bool IsBombAtTile(int x, int y)
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (bombs[i].active && bombs[i].tileX == x && bombs[i].tileY == y)
            return true;
    }
    return false;
}


bool CheckWallCollision(Vector2 nextPos, Player p)
{
    int m = 8;
    int l = (nextPos.x + m) / TILE_SIZE, r = (nextPos.x + TILE_SIZE - m) / TILE_SIZE;
    int t = (nextPos.y - 60 + m) / TILE_SIZE, b = (nextPos.y - 60 + TILE_SIZE - m) / TILE_SIZE;

    if (l < 0 || r >= MAP_WIDTH || t < 0 || b >= MAP_HEIGHT)
        return true;
    if (currentMap[t][l] || currentMap[t][r] || currentMap[b][l] || currentMap[b][r])
        return true;

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
// Ajouté par Abdoulaye : Gestion des explosions
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
// Ajouté par Abdoulaye : Déclenchement d'une explosion et propagation

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
// Fin des fonctions d'Abdoulaye
int main(void)
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bomberman Mini");
    SetTargetFPS(FPS);

    // =========================================================================
    // 🛠️ FIX 1 : CHARGEMENT UNIQUE DES TEXTURES AVANT LA BOUCLE (Plus de fuite mémoire !)
    // =========================================================================
    Texture2D texPlayer = LoadTexture("../intro/images/bonhomme.png");

    float frameWidth = (float)texPlayer.width / 3;
    float frameHeight = (float)texPlayer.height / 4;
    Rectangle sourceRec = {0.0f, 0.0f, frameWidth, frameHeight};

    Texture2D texFloor = LoadTexture("../intro/images/herbe.png");
    Texture2D texWall = LoadTexture("../intro/images/murgris.png");
    Texture2D texCrate = LoadTexture("../intro/images/tonneau.png");

    Texture2D texBomb = {0};
    Texture2D texFire = {0};
    if (FileExists("../intro/images/bomb.png"))
        texBomb = LoadTexture("../intro/images/bomb.png");
    if (FileExists("../intro/images/fire.png"))
        texFire = LoadTexture("../intro/images/fire.png");

    // Variables d'animation reliées à la boucle principale
    int currentFrame = 0;
    float frameCounter = 0.0f;
    float frameSpeed = 12.0f;

    GameScreen currentScreen = SCREEN_MENU;
    Player player = {(Vector2){TILE_SIZE, TILE_SIZE + 60}, 2.5f, 1, DIR_DOWN, false, 1, 0.0f};
    currentMap = level1;

    // =========================================================================
    // 🛠️ FIX 2 : UNE SEULE ET UNIQUE BOUCLE PRINCIPALE (Fin du conflit de boucles)
    // =========================================================================
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        switch (currentScreen)
        {
        case SCREEN_MENU:
            if (IsKeyPressed(KEY_ENTER))
                currentScreen = SCREEN_GAMEPLAY;
            break;
            // Le reste du code de la boucle principale est dans le fichier jeufinal.c pour éviter les conflits de merge

        case SCREEN_GAMEPLAY:
        {
            Vector2 nextPos = player.position;
            player.isMoving = false;

            // Inputs et direction
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

            // Résolution des collisions
            if (!CheckWallCollision((Vector2){nextPos.x, player.position.y}, player))
                player.position.x = nextPos.x;
            if (!CheckWallCollision((Vector2){player.position.x, nextPos.y}, player))
                player.position.y = nextPos.y;

            // 🛠️ FIX 3 : ANIMATION DYNAMIQUE AVEC LA FEUILLE DE SPRITE
            if (player.isMoving)
            {
                frameCounter += dt;
                if (frameCounter >= (1.0f / frameSpeed))
                {
                    frameCounter = 0.0f;
                    currentFrame = (currentFrame + 1) % 3; // Défilement horizontal (3 frames)
                }
            }
            else
            {
                currentFrame = 1; // Frame du milieu (immobile)
            }

            // Application de la découpe sur la feuille de texture
            sourceRec.x = currentFrame * frameWidth;
            sourceRec.y = player.dir * frameHeight; // Ligne gérée par la direction du joueur !

            // Poser une bombe
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

            // Logique des bombes actives
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

            // Logique des explosions
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosions[i].active)
                {
                    explosions[i].timer -= dt;
                    if (explosions[i].timer <= 0.0f)
                        explosions[i].active = false;
                }
            }

            // Changements de niveaux / triche
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

        // =========================================================================
        // RENDU GRAPHIQUE (DRAW)
        // =========================================================================
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScreen == SCREEN_MENU)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, DARKBLUE);
            DrawText("WELCOME TO BOMBERMAN", SCREEN_WIDTH / 2 - MeasureText("WELCOME TO BOMBERMAN", 30) / 2, SCREEN_HEIGHT / 3, 30, YELLOW);
        }
        else if (currentScreen == SCREEN_GAMEPLAY)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, 60, BLACK);
            DrawText(TextFormat("NIVEAU: %d", player.currentLevel), 20, 20, 20, GOLD);

            // Cartographie des sols et blocs
            for (int y = 0; y < MAP_HEIGHT; y++)
            {
                for (int x = 0; x < MAP_WIDTH; x++)
                {
                    float px = x * TILE_SIZE, py = y * TILE_SIZE + 60;
                    DrawTexturePro(texFloor, (Rectangle){0, 0, texFloor.width, texFloor.height}, (Rectangle){px, py, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                    if (currentMap[y][x] == 1)
                        DrawTexturePro(texWall, (Rectangle){0, 0, texWall.width, texWall.height}, (Rectangle){px, py, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                    if (currentMap[y][x] == 2)
                        DrawTexturePro(texCrate, (Rectangle){0, 0, texCrate.width, texCrate.height}, (Rectangle){px, py, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
                }
            }

            // Rendu des bombes
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

            // Rendu des explosions
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

            // 🛠️ FIX 4 : LE JOUEUR EST ENFIN DESSINÉ AVEC SA FRAME ET SA DIRECTION REELLES !
            DrawTexturePro(texPlayer, sourceRec, (Rectangle){player.position.x, player.position.y, TILE_SIZE, TILE_SIZE}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (currentScreen == SCREEN_WIN)
        {
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GREEN);
            DrawText("VICTOIRE !", SCREEN_WIDTH / 2 - MeasureText("VICTOIRE !", 40) / 2, SCREEN_HEIGHT / 3, 40, WHITE);
        }

        EndDrawing();
    }

    // Libération correcte de la mémoire à la fermeture du jeu
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