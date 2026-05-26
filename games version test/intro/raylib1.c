#include <stdio.h>
#include "raylib.h"
#include "math.h"

// =========================================================================
// 📐 CONFIGURATION ET CONSTANTES
// =========================================================================
#define TAILLE_CASE 40
#define LARGEUR_CARTE 15
#define HAUTEUR_CARTE 13
#define LARGEUR_ECRAN (LARGEUR_CARTE * TAILLE_CASE)
#define HAUTEUR_ECRAN (HAUTEUR_CARTE * TAILLE_CASE + 60) // +60px pour le bandeau d'interface
#define MAX_BOMBES 3
#define TEMPS_MECHE_BOMBE 2.5f
#define TEMPS_EXPLOSION 0.5f
#define RAYON_BOMBE 2
#define MAX_EXPLOSIONS 50
#define FPS 60

// =========================================================================
// 🎭 ENUMS ET STRUCTURES
// =========================================================================

// États des écrans de jeu
typedef enum
{
    ECRAN_MENU,
    ECRAN_JEU,
    ECRAN_VICTOIRE
} EcranJeu;

// Directions pour les déplacements et animations du joueur
typedef enum
{
    DIR_BAS,
    DIR_GAUCHE,
    DIR_DROITE,
    DIR_HAUT
} Direction;

// Structure du joueur
typedef struct
{
    Vector2 position;
    float vitesse;
    int niveauActuel;
    Direction direction;
    bool enMouvement;
    int imageActuelle;
} Joueur;

// Structure pour la gestion des bombes
typedef struct
{
    int caseX;
    int caseY;
    float minuteur;
    bool active;
} Bombe;

// Structure pour la gestion des particules d'explosion
typedef struct
{
    int caseX;
    int caseY;
    float minuteur;
    bool active;
} Explosion;

// =========================================================================
// 🗺️ MATRICES ET CARTES DES NIVEAUX (0: Vide, 1: Incassable, 2: Caisse, 3: Bloc Doré)
// =========================================================================
int niveau1[HAUTEUR_CARTE][LARGEUR_CARTE] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 3, 2, 2, 0, 0, 0, 2, 2, 2, 0, 0, 1}, // MODIF: Remplacement d'une caisse par un bloc doré (3)
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

int niveau2[HAUTEUR_CARTE][LARGEUR_CARTE] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 1, 3, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1}, // MODIF: Remplacement d'une caisse par un bloc doré (3)
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

// Variables globales de jeu
int (*carteActuelle)[LARGEUR_CARTE];
Bombe bombes[MAX_BOMBES] = {0};
Explosion explosions[MAX_EXPLOSIONS] = {0};
bool gagneParBloc = false; // MODIF: Flag global pour déclencher la victoire

// =========================================================================
// ⚙️ FONCTIONS COMPORTEMENTALES (LOGIQUE)
// =========================================================================

// Vérifie si une bombe est déjà présente sur une case spécifique
bool EstBombeSurCase(int x, int y)
{
    for (int i = 0; i < MAX_BOMBES; i++)
    {
        if (bombes[i].active && bombes[i].caseX == x && bombes[i].caseY == y)
            return true;
    }
    return false;
}

// Gère les collisions avec l'environnement (murs, bordures, bombes)
bool VerifierCollisionMur(Vector2 prochainePos, Joueur j)
{
    int marge = 8; // Marge d'ajustement pour fluidifier les virages dans les couloirs
    int gauche = (prochainePos.x + marge) / TAILLE_CASE;
    int droite = (prochainePos.x + TAILLE_CASE - marge) / TAILLE_CASE;
    int haut = (prochainePos.y - 60 + marge) / TAILLE_CASE; // Ajustement lié au bandeau de 60px
    int bas = (prochainePos.y - 60 + TAILLE_CASE - marge) / TAILLE_CASE;

    // Détection des limites extérieures de la carte
    if (gauche < 0 || droite >= LARGEUR_CARTE || haut < 0 || bas >= HAUTEUR_CARTE)
        return true;

    // Détection des obstacles solides (1: Murs, 2: Caisses, 3: Bloc Doré détecté automatiquement car non-nul)
    if (carteActuelle[haut][gauche] || carteActuelle[haut][droite] || carteActuelle[bas][gauche] || carteActuelle[bas][droite])
        return true;

    // Calcul de la case actuelle occupée par le centre du joueur
    int joueurX = (j.position.x + TAILLE_CASE / 2) / TAILLE_CASE;
    int joueurY = (j.position.y - 60 + TAILLE_CASE / 2) / TAILLE_CASE;

    // Bloquer le joueur sur les bombes externes (il peut marcher sur sa propre bombe tant qu'il ne la quitte pas)
    if (EstBombeSurCase(gauche, haut) && (gauche != joueurX || haut != joueurY))
        return true;
    if (EstBombeSurCase(droite, haut) && (droite != joueurX || haut != joueurY))
        return true;
    if (EstBombeSurCase(gauche, bas) && (gauche != joueurX || bas != joueurY))
        return true;
    if (EstBombeSurCase(droite, bas) && (droite != joueurX || bas != joueurY))
        return true;

    return false;
}

// Enregistre une nouvelle cellule d'explosion active dans la file
void AjouterExplosion(int x, int y)
{
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
    {
        if (!explosions[i].active)
        {
            // Correction syntaxique de l'initialisation de structure pour compatibilité standard
            explosions[i].caseX = x;
            explosions[i].caseY = y;
            explosions[i].minuteur = TEMPS_EXPLOSION;
            explosions[i].active = true;
            break;
        }
    }
}

// Déclenche l'onde d'explosion en croix (Haut, Bas, Gauche, Droite)
void DeclencherExplosion(int centreX, int centreY)
{
    AjouterExplosion(centreX, centreY);

    int dirX[] = {0, 0, -1, 1};
    int dirY[] = {-1, 1, 0, 0};

    for (int i = 0; i < 4; i++)
    {
        for (int r = 1; r <= RAYON_BOMBE; r++)
        {
            int cibleX = centreX + dirX[i] * r;
            int cibleY = centreY + dirY[i] * r;

            // Limitation aux bordures du terrain
            if (cibleX < 0 || cibleX >= LARGEUR_CARTE || cibleY < 0 || cibleY >= HAUTEUR_CARTE)
                break;
            // Arrêt immédiat si impact avec un mur indestructible
            if (carteActuelle[cibleY][cibleX] == 1)
                break;
            // Destruction de la caisse et arrêt du souffle de l'explosion
            if (carteActuelle[cibleY][cibleX] == 2)
            {
                carteActuelle[cibleY][cibleX] = 0;
                AjouterExplosion(cibleX, cibleY);
                break;
            }
            // MODIF: Destruction du bloc doré -> active le flag de victoire
            if (carteActuelle[cibleY][cibleX] == 3)
            {
                carteActuelle[cibleY][cibleX] = 0;
                AjouterExplosion(cibleX, cibleY);
                gagneParBloc = true;
                break;
            }
            // Propagation normale sur sol vide
            if (carteActuelle[cibleY][cibleX] == 0)
                AjouterExplosion(cibleX, cibleY);
        }
    }
}

// =========================================================================
// 🚀 POINT D'ENTRÉE PRINCIPAL
// =========================================================================
int main(void)
{
    // Initialisation de la fenêtre graphique Raylib
    InitWindow(LARGEUR_ECRAN, HAUTEUR_ECRAN, "Bomberman Mini");
    SetTargetFPS(FPS);

    // Chargement initial sécurisé de l'ensemble des textures
    Texture2D texJoueur = LoadTexture("../intro/images/bonhomme.png");
    float largeurImage = (float)texJoueur.width / 3;
    float hauteurImage = (float)texJoueur.height / 4;
    Rectangle rectSource = {0.0f, 0.0f, largeurImage, hauteurImage};

    // Syntaxe d'initialisation explicite pour compatibilité stricte
    Bombe b_init = {0, 0, 0.0f, false};
    for (int i = 0; i < MAX_BOMBES; i++)
        bombes[i] = b_init;
    Explosion e_init = {0, 0, 0.0f, false};
    for (int i = 0; i < MAX_EXPLOSIONS; i++)
        explosions[i] = e_init;

    Texture2D texSol = LoadTexture("../intro/images/herbe.png");
    Texture2D texMur = LoadTexture("../intro/images/murgris.png");
    Texture2D texCaisse = LoadTexture("../intro/images/tonneau.png");
    

    Texture2D texBombe = {0};
    Texture2D texFeu = {0};
    if (FileExists("../intro/images/bomb.png"))
        texBombe = LoadTexture("../intro/images/bomb.png");
    if (FileExists("../intro/images/fire.png"))
        texFeu = LoadTexture("../intro/images/fire.png");

    // Initialisation des états d'animation du personnage
    int indexImage = 0;
    float compteurImage = 0.0f;
    float vitesseAnim = 12.0f;

    // Configuration de départ des entités
    EcranJeu ecranActuel = ECRAN_MENU;
    Joueur joueur = {(Vector2){TAILLE_CASE, TAILLE_CASE + 60}, 2.5f, 1, DIR_BAS, false, 1};
    carteActuelle = niveau1;

    // ---------------------------------------------------------------------
    // 🔄 BOUCLE PRINCIPALE UNIQUE
    // ---------------------------------------------------------------------
    while (!WindowShouldClose())
    {
        float tempsDelta = GetFrameTime();

        // --- SECTION UPDATE : LOGIQUE DE JEU ---
        switch (ecranActuel)
        {
        case ECRAN_MENU:
            if (IsKeyPressed(KEY_ENTER))
                ecranActuel = ECRAN_JEU;
            break;

        case ECRAN_JEU:
        {
            // MODIF: Si le bloc doré a été détruit, on passe à l'écran de victoire
            if (gagneParBloc)
            {
                ecranActuel = ECRAN_VICTOIRE;
                gagneParBloc = false;
            }

            Vector2 prochainePos = joueur.position;
            joueur.enMouvement = false;

            // Captures des entrées clavier et assignation de la direction
            if (IsKeyDown(KEY_RIGHT))
            {
                prochainePos.x += joueur.vitesse;
                joueur.direction = DIR_DROITE;
                joueur.enMouvement = true;
            }
            else if (IsKeyDown(KEY_LEFT))
            {
                prochainePos.x -= joueur.vitesse;
                joueur.direction = DIR_GAUCHE;
                joueur.enMouvement = true;
            }
            else if (IsKeyDown(KEY_DOWN))
            {
                prochainePos.y += joueur.vitesse;
                joueur.direction = DIR_BAS;
                joueur.enMouvement = true;
            }
            else if (IsKeyDown(KEY_UP))
            {
                prochainePos.y -= joueur.vitesse;
                joueur.direction = DIR_HAUT;
                joueur.enMouvement = true;
            }

            // Application des déplacements par axes s'il n'y a pas de collision
            if (!VerifierCollisionMur((Vector2){prochainePos.x, joueur.position.y}, joueur))
                joueur.position.x = prochainePos.x;
            if (!VerifierCollisionMur((Vector2){joueur.position.x, prochainePos.y}, joueur))
                joueur.position.y = prochainePos.y;

            // Calcul dynamique des frames de l'animation du sprite
            if (joueur.enMouvement)
            {
                compteurImage += tempsDelta;
                if (compteurImage >= (1.0f / vitesseAnim))
                {
                    compteurImage = 0.0f;
                    indexImage = (indexImage + 1) % 3; // Défilement horizontal cyclique (3 frames)
                }
            }
            else
            {
                indexImage = 1; // Position stable par défaut à l'arrêt
            }

            // Définition de la portion de texture à découper
            rectSource.x = indexImage * largeurImage;
            rectSource.y = joueur.direction * hauteurImage;

            // Logique pour poser une bombe (Touche Espace)
            if (IsKeyPressed(KEY_SPACE))
            {
                int caseX = (joueur.position.x + TAILLE_CASE / 2) / TAILLE_CASE;
                int caseY = (joueur.position.y - 60 + TAILLE_CASE / 2) / TAILLE_CASE;
                if (!EstBombeSurCase(caseX, caseY))
                {
                    for (int i = 0; i < MAX_BOMBES; i++)
                    {
                        if (!bombes[i].active)
                        {
                            bombes[i].caseX = caseX;
                            bombes[i].caseY = caseY;
                            bombes[i].minuteur = TEMPS_MECHE_BOMBE;
                            bombes[i].active = true;
                            break;
                        }
                    }
                }
            }

            // Actualisation des minuteurs des bombes posées
            for (int i = 0; i < MAX_BOMBES; i++)
            {
                if (bombes[i].active)
                {
                    bombes[i].minuteur -= tempsDelta;
                    if (bombes[i].minuteur <= 0.0f)
                    {
                        bombes[i].active = false;
                        DeclencherExplosion(bombes[i].caseX, bombes[i].caseY);
                    }
                }
            }

            // Actualisation de la durée de vie des explosions actives
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosions[i].active)
                {
                    explosions[i].minuteur -= tempsDelta;
                    if (explosions[i].minuteur <= 0.0f)
                        explosions[i].active = false;
                }
            }

            // Raccourcis développeurs / triche de progression
            if (IsKeyPressed(KEY_N) && joueur.niveauActuel == 1)
            {
                joueur.niveauActuel = 2;
                carteActuelle = niveau2;
                niveau2[2][3] = 3; // MODIF: Régénère le bloc doré du niveau 2
                joueur.position = (Vector2){TAILLE_CASE, TAILLE_CASE + 60};
            }
            if (IsKeyPressed(KEY_G))
                ecranActuel = ECRAN_VICTOIRE;
        }
        break;

        case ECRAN_VICTOIRE:
            if (IsKeyPressed(KEY_ENTER))
            {
                joueur.niveauActuel = 1;
                carteActuelle = niveau1;
                niveau1[1][3] = 3; // MODIF: Réinitialise le bloc doré du niveau 1 pour la prochaine partie
                niveau2[2][3] = 3; // MODIF: Réinitialise le bloc doré du niveau 2 pour la prochaine partie
                joueur.position = (Vector2){TAILLE_CASE, TAILLE_CASE + 60};
                joueur.direction = DIR_BAS;
                ecranActuel = ECRAN_MENU;
            }
            break;
        }

        // --- SECTION DRAW : RENDU VISUEL ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (ecranActuel == ECRAN_MENU)
        {
            DrawRectangle(0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN, DARKBLUE);
            DrawText("WELCOME TO BOMBERMAN", LARGEUR_ECRAN / 2 - MeasureText("WELCOME TO BOMBERMAN", 30) / 2, HAUTEUR_ECRAN / 3, 30, YELLOW);
        }
        else if (ecranActuel == ECRAN_JEU)
        {
            // Dessin de la barre d'outils / UI
            DrawRectangle(0, 0, LARGEUR_ECRAN, 60, BLACK);
            DrawText(TextFormat("NIVEAU: %d", joueur.niveauActuel), 20, 20, 20, GOLD);

            // Génération graphique du sol et des modules de blocs
            for (int y = 0; y < HAUTEUR_CARTE; y++)
            {
                for (int x = 0; x < LARGEUR_CARTE; x++)
                {
                    float posX = x * TAILLE_CASE, posY = y * TAILLE_CASE + 60;

                    DrawTexturePro(texSol, (Rectangle){0, 0, texSol.width, texSol.height}, (Rectangle){posX, posY, TAILLE_CASE, TAILLE_CASE}, (Vector2){0, 0}, 0.0f, WHITE);
                    if (carteActuelle[y][x] == 1)
                        DrawTexturePro(texMur, (Rectangle){0, 0, texMur.width, texMur.height}, (Rectangle){posX, posY, TAILLE_CASE, TAILLE_CASE}, (Vector2){0, 0}, 0.0f, WHITE);
                    if (carteActuelle[y][x] == 2)
                        DrawTexturePro(texCaisse, (Rectangle){0, 0, texCaisse.width, texCaisse.height}, (Rectangle){posX, posY, TAILLE_CASE, TAILLE_CASE}, (Vector2){0, 0}, 0.0f, WHITE);

                    // MODIF: Rendu visuel du bloc doré (Un rectangle couleur OR)
                    if (carteActuelle[y][x] == 3)
                        DrawRectangle(posX, posY, TAILLE_CASE, TAILLE_CASE, GOLD);
                }
            }

            // Affichage graphique des bombes (avec un effet d'échelle pulsatif)
            for (int i = 0; i < MAX_BOMBES; i++)
            {
                if (bombes[i].active)
                {
                    float pulse = 1.0f + (sinf(bombes[i].minuteur * 10.0f) * 0.05f);
                    float posX_bombe = bombes[i].caseX * TAILLE_CASE + (TAILLE_CASE - TAILLE_CASE * pulse) / 2;
                    float posY_bombe = bombes[i].caseY * TAILLE_CASE + 60 + (TAILLE_CASE - TAILLE_CASE * pulse) / 2;

                    if (texBombe.width > 0)
                        DrawTexturePro(texBombe, (Rectangle){0, 0, texBombe.width, texBombe.height}, (Rectangle){posX_bombe, posY_bombe, TAILLE_CASE * pulse, TAILLE_CASE * pulse}, (Vector2){0, 0}, 0.0f, WHITE);
                    else
                        DrawCircleV((Vector2){posX_bombe + TAILLE_CASE * pulse / 2, posY_bombe + TAILLE_CASE * pulse / 2}, (TAILLE_CASE * pulse) * 0.35f, BLACK);
                }
            }

            // Affichage des tuiles d'explosions actives
            for (int i = 0; i < MAX_EXPLOSIONS; i++)
            {
                if (explosions[i].active)
                {
                    if (texFeu.width > 0)
                        DrawTexturePro(texFeu, (Rectangle){0, 0, texFeu.width, texFeu.height}, (Rectangle){explosions[i].caseX * TAILLE_CASE, explosions[i].caseY * TAILLE_CASE + 60, TAILLE_CASE, TAILLE_CASE}, (Vector2){0, 0}, 0.0f, WHITE);
                    else
                        DrawRectangle(explosions[i].caseX * TAILLE_CASE, explosions[i].caseY * TAILLE_CASE + 60, TAILLE_CASE, TAILLE_CASE, ORANGE);
                }
            }

            // Rendu finalisé du joueur avec sa découpe de sprite correspondante
            DrawTexturePro(texJoueur, rectSource, (Rectangle){joueur.position.x, joueur.position.y, TAILLE_CASE, TAILLE_CASE}, (Vector2){0, 0}, 0.0f, WHITE);
        }
        else if (ecranActuel == ECRAN_VICTOIRE)
        {
            DrawRectangle(0, 0, LARGEUR_ECRAN, HAUTEUR_ECRAN, GREEN);
            DrawText("VICTOIRE !", LARGEUR_ECRAN / 2 - MeasureText("VICTOIRE !", 40) / 2, HAUTEUR_ECRAN / 3, 40, WHITE);
        }

        EndDrawing();
    }

    // Unload / Libération propre des ressources graphiques de la VRAM
    UnloadTexture(texSol);
    UnloadTexture(texMur);
    UnloadTexture(texCaisse);
    UnloadTexture(texJoueur);
    if (texBombe.width > 0)
        UnloadTexture(texBombe);
    if (texFeu.width > 0)
        UnloadTexture(texFeu);

    CloseWindow();
    return 0;
}