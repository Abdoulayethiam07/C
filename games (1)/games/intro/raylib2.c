#include <time.h>
#include "raylib.h"
#include "raymath.h"
#define SCREEN_WIDHT 700
#define SCREEN_HEIGHT 650
#define FPS 60
#define vitesse_initiale 5

// Partie Des fonctions et des structure.
typedef enum
{
    MENU,
    PLAY,
    GAMEOVER
} Game_Etat;

typedef struct plage
{
    int min;
    int max;
} plage;

typedef struct
{
    int x1;
    int x2;
    int x3;
    int y1;
    int y2;
    int y3;
    struct plage plages;
} Obstacle;

void calcule_position(int min_y, int max_y, Obstacle *position)
{
    int a;

    position->x1 = GetRandomValue(130, 220);
    position->x2 = GetRandomValue(280, 370);
    position->x3 = GetRandomValue(430, 520);
    position->y1 = GetRandomValue(min_y, max_y);
    position->y2 = GetRandomValue(min_y, max_y);
    position->y3 = GetRandomValue(min_y, max_y);
}

int main(void)
{

    InitWindow(SCREEN_WIDHT, SCREEN_HEIGHT, "Jeux");
    SetTargetFPS(FPS);
    SetRandomSeed(time(NULL));

    Game_Etat ETAT = MENU;

    // Gestion du score
    int temps = 0;
    int distance;
    int checkpoint = 0;
    int vitesse = 5;
    int vitesse_mouvement = 5;
    int score = 0;
    int score_p = 0;

    // variable voiture du joueur et colision
    Vector2 position_initiale = {328, 550};
    Rectangle voiture_player;
    bool collisions[9];
    bool collision = false;

    // Variable de generation des obstacle

    int a;
    int c = 0;
    bool initialisaton = false;
    Obstacle Groupe_Obstacle[3];
    Rectangle destination[9];
    Texture2D images[9];
    Rectangle sources[9];
    Vector2 origin = {0, 0};

    // chargement des texture
    images[0] = LoadTexture("image/camion_bleu.png");
    images[1] = LoadTexture("image/camion_rouge.png");
    images[2] = LoadTexture("image/moto.png");
    images[3] = LoadTexture("image/voiture_bleu.png");
    images[4] = LoadTexture("image/voiture_jaune.png");
    images[5] = LoadTexture("image/quad.png");
    images[6] = LoadTexture("image/voiture_verte.png");
    images[7] = LoadTexture("image/voiture_viollete.png");
    images[8] = LoadTexture("image/bus.png");

    Texture2D text_player = LoadTexture("image/voiture_player.png");
    Rectangle source_player = (Rectangle){0.0f, 0.0f, (float)text_player.width, (float)text_player.height};

    for (a = 0; a < 9; a++)
    {
        sources[a] = (Rectangle){0.0f, 0.0f, (float)images[a].width, (float)images[a].height};
    }

    // Variable de generation de la route

    int axe_y_verdure[3] = {325, 0, -325};
    int axe_y_bordure[20] = {610, 570, 530, 490, 450, 410, 370, 330, 290, 250, 210, 170, 130, 90, 50, 10, -30, -70, -110, -150};
    int axe_y_bande[9] = {550, 450, 350, 250, 150, 50, -50, -150, -250};

    while (!WindowShouldClose())
    {

        voiture_player = (Rectangle){position_initiale.x, position_initiale.y, 45, 70};

        while (!initialisaton)
        {

            for (a = 0; a < 3; a++)
            {
                c -= 700;
                Groupe_Obstacle[a].plages.min = c;
                Groupe_Obstacle[a].plages.max = c + 700;
                calcule_position(Groupe_Obstacle[a].plages.min - 80, Groupe_Obstacle[a].plages.max - 80, &Groupe_Obstacle[a]);
            }
            initialisaton = true;
        }

        // mouvement de la voiture

        if (ETAT == PLAY)
        {

            if (IsKeyDown(KEY_UP) && position_initiale.y > 0)
            {
                position_initiale.y -= vitesse_mouvement;
            }
            if (IsKeyDown(KEY_DOWN) && position_initiale.y < 580)
            {
                position_initiale.y += vitesse_mouvement;
            }
            if (IsKeyDown(KEY_LEFT) && position_initiale.x > 110)
            {
                position_initiale.x -= vitesse_mouvement;
            }
            if (IsKeyDown(KEY_RIGHT) && position_initiale.x < 540)
            {
                position_initiale.x += vitesse_mouvement;
            }

            for (a = 0; a < 3; a++)
            {
                Groupe_Obstacle[a].y1 += vitesse;
                Groupe_Obstacle[a].y2 += vitesse;
                Groupe_Obstacle[a].y3 += vitesse;
                Groupe_Obstacle[a].plages.min += vitesse;
                Groupe_Obstacle[a].plages.max += vitesse;

                if (Groupe_Obstacle[a].plages.min >= SCREEN_HEIGHT)
                {
                    Groupe_Obstacle[a].plages.min -= 2250;
                    Groupe_Obstacle[a].plages.max = Groupe_Obstacle[a].plages.min + 700;
                    calcule_position(Groupe_Obstacle[a].plages.min, Groupe_Obstacle[a].plages.max, &Groupe_Obstacle[a]);
                }
            }

            for (a = 0; a < 3; a++)
            {
                axe_y_verdure[a] += vitesse;
                if (axe_y_verdure[a] >= SCREEN_HEIGHT)
                {
                    axe_y_verdure[a] -= 975;
                }
            }

            for (a = 0; a < 20; a++)
            {
                axe_y_bordure[a] += vitesse;
                if (axe_y_bordure[a] >= SCREEN_HEIGHT)
                {
                    axe_y_bordure[a] -= 800;
                }
            }

            for (a = 0; a < 9; a++)
            {
                axe_y_bande[a] += vitesse;
                if (axe_y_bande[a] >= SCREEN_HEIGHT)
                {
                    axe_y_bande[a] -= 900;
                }
            }

            destination[0] = (Rectangle){Groupe_Obstacle[0].x1, Groupe_Obstacle[0].y1, 45, 90};
            destination[1] = (Rectangle){Groupe_Obstacle[0].x2, Groupe_Obstacle[0].y2, 45, 90};
            destination[2] = (Rectangle){Groupe_Obstacle[0].x3, Groupe_Obstacle[0].y3, 35, 90};

            destination[3] = (Rectangle){Groupe_Obstacle[1].x1, Groupe_Obstacle[1].y1, 45, 90};
            destination[4] = (Rectangle){Groupe_Obstacle[1].x2, Groupe_Obstacle[1].y2, 45, 90};
            destination[5] = (Rectangle){Groupe_Obstacle[1].x3, Groupe_Obstacle[1].y3, 45, 80};

            destination[6] = (Rectangle){Groupe_Obstacle[2].x1, Groupe_Obstacle[2].y1, 45, 90};
            destination[7] = (Rectangle){Groupe_Obstacle[2].x2, Groupe_Obstacle[2].y2, 45, 90};
            destination[8] = (Rectangle){Groupe_Obstacle[2].x3, Groupe_Obstacle[2].y3, 45, 110};

            for (a = 0; a < 9; a++)
            {
                collisions[a] = CheckCollisionRecs(voiture_player, destination[a]);
                if (collisions[a])
                {
                    vitesse = 0;
                    vitesse_mouvement = 0;
                    // collision = true;
                    ETAT = GAMEOVER;
                }
            }

            distance = vitesse * temps;
            if (distance - checkpoint >= 325)
            {
                score++;
                checkpoint = distance;
                if (score - score_p >= 10)
                {
                    vitesse++;
                    score_p = score;
                }
            }
            temps++;
        }

        BeginDrawing();

        ClearBackground(BLACK);

        if (ETAT == PLAY)
        {

            // Affichage de la route
            for (a = 0; a < 3; a++)
            {
                DrawRectangle(0, axe_y_verdure[a], 80, 325, GREEN);
                DrawRectangle(620, axe_y_verdure[a], 80, 325, GREEN);
            }
            for (a = 0; a < 20; a++)
            {
                if (a % 2 == 0)
                {
                    DrawRectangle(80, axe_y_bordure[a], 30, 40, RED);
                    DrawRectangle(590, axe_y_bordure[a], 30, 40, RED);
                }
                else
                {
                    DrawRectangle(80, axe_y_bordure[a], 30, 40, WHITE);
                    DrawRectangle(590, axe_y_bordure[a], 30, 40, WHITE);
                }
            }
            for (a = 0; a < 9; a++)
            {
                DrawRectangle(341, axe_y_bande[a], 18, 70, WHITE);
            }

            // Affichage des obstacle

            for (a = 0; a < 9; a++)
            {
                DrawTexturePro(images[a], sources[a], destination[a], origin, 0, WHITE);
            }

            DrawTexturePro(text_player, source_player, voiture_player, origin, 0, WHITE);

            DrawText(TextFormat("Score: %d", score), 470, 20, 20, GREEN);
        }

        else if (ETAT == MENU)
        {
            DrawText("BIENVENUE", 290, 80, 20, WHITE);
            DrawText("Cliquez sur ENTRER pour jouer et sur ECHAP pour sortir ! ", 30, 200, 20, WHITE);
            DrawText("JOUER", 160, 300, 30, GREEN);
            DrawText("SORTIE", 420, 300, 30, RED);

            if (IsKeyPressed(KEY_ENTER))
            {
                ETAT = PLAY;
            }
        }

        if (ETAT == GAMEOVER)
        {
            DrawText("GAME OVER", 200, 100, 40, WHITE);
            DrawText(TextFormat("SCORE: %d", score), 240, 200, 35, WHITE);
            DrawText("Cliquez sur ENTRER pour rejouer et sur ECHAP pour sortir ! ", 30, 300, 20, WHITE);
            DrawText("REJOUEZ", 160, 400, 30, GREEN);
            DrawText("SORTIE", 420, 400, 30, RED);

            if (IsKeyPressed(KEY_ENTER))
            {

                vitesse = vitesse_initiale;
                vitesse_mouvement = 5;
                score = 0;
                score_p = 0;
                ETAT = PLAY;
                initialisaton = false;
                c = 0;
                position_initiale = (Vector2){328, 550};
                temps = 0;
                checkpoint = 0;
                SetRandomSeed(time(NULL));
            }
        }
        EndDrawing();
    }

    CloseWindow();
}

// Je dois encore creer un espacement entre les zone pour évité les obstacle trop colé