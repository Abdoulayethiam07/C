#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// =============================================================================
// CONSTANTES ET CONFIGURATION
// =============================================================================
#define MAP_WIDTH 19   // Largeur de la grille (nombre de cases)
#define MAP_HEIGHT 13  // Hauteur de la grille
#define MAX_BOMBS 10   // Nombre maximum de bombes simultanées à l'écran
#define MAX_ENEMIES 20 // Nombre maximum d'ennemis à l'écran

// =============================================================================
// ENUMS (Pour rendre le code explicite)
// =============================================================================
typedef enum
{
    TILE_EMPTY,    // Case vide où l'on peut marcher
    TILE_WALL,     // Bloc indestructible (les piliers gris)
    TILE_BRICK,    // Bloc destructible (les barils/buissons)
    TILE_EXPLOSION // Case actuellement occupée par du feu
} TileType;

typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
} Direction;

// =============================================================================
// STRUCTURES DE DONNÉES
// =============================================================================

// Position générique (en cases de grille ou en pixels selon ton choix futur)
typedef struct
{
    int x;
    int y;
} Position;

// Structure représentant le Joueur (Bomberman)
typedef struct
{
    Position pos;        // Position actuelle sur la grille
    int lives;           // Nombre de vies restantes
    int bomb_capacity;   // Nombre max de bombes qu'il peut poser en même temps
    int explosion_range; // Portée de ses explosions (en nombre de cases)
    int score;           // Score du joueur
    bool is_alive;       // Statut de vie
} Player;

// Structure représentant une Bombe
typedef struct
{
    Position pos;   // Position de la bombe
    int timer;      // Temps restant avant explosion (ex: en frames ou millisecondes)
    int range;      // Portée de cette bombe spécifique
    bool is_active; // Est-ce que la bombe est posée ?
} Bomb;

// Structure représentant un Ennemi
typedef struct
{
    Position pos;  // Position de l'ennemi
    Direction dir; // Direction actuelle de son déplacement
    bool is_alive; // Statut
    int type;      // Type d'ennemi (vitesse, comportement différents)
} Enemy;

// Le "Cerveau" du jeu : Regroupe TOUTES les variables d'état
typedef struct
{
    TileType map[MAP_HEIGHT][MAP_WIDTH]; // La matrice 2D du niveau
    Player player;                       // L'instance du joueur
    Bomb bombs[MAX_BOMBS];               // Tableau de bombes gérées par le jeu
    Enemy enemies[MAX_ENEMIES];          // Tableau des ennemis présents
    int active_enemies_count;            // Nombre d'ennemis encore en vie
    int game_time;                       // Temps restant pour le niveau (le timer en haut)
    bool is_running;                     // Contrôle la boucle principale
} GameState;

// =============================================================================
// PROTOTYPES DES FONCTIONS DE LA BOUCLE DE JEU
// =============================================================================
void init_game(GameState *game);
void handle_input(GameState *game);
void update_game(GameState *game);
void render_game(const GameState *game);
void clean_game(GameState *game);

// =============================================================================
// FONCTION PRINCIPALE (Point d'entrée)
// =============================================================================
int main(void)
{
    GameState game;

    // 1. Initialisation du jeu et chargement de la map
    init_game(&game);

    // 2. La "Game Loop" principale (S'exécute en continu)
    while (game.is_running)
    {
        handle_input(&game); // Écouter le clavier / manette
        update_game(&game);  // Mettre à jour la physique, timers, IA, collisions
        render_game(&game);  // Dessiner le résultat à l'écran
    }

    // 3. Nettoyage avant de quitter
    clean_game(&game);

    return EXIT_SUCCESS;
}

// =============================================================================
// IMPLÉMENTATIONS DE BASE (À compléter)
// =============================================================================

/**
 * Initialise les structures de données, place les blocs fixes,
 * configure le joueur et fait apparaître les ennemis.
 */
void init_game(GameState *game)
{
    game->is_running = true;
    game->game_time = 195; // Comme sur la capture (TIME 195)

    // Initialisation du joueur
    game->player.pos.x = 9; // Position centrale approximative
    game->player.pos.y = 1;
    game->player.lives = 3;
    game->player.bomb_capacity = 1;
    game->player.explosion_range = 2;
    game->player.score = 0;
    game->player.is_alive = true;

    // Remplissage de la carte par défaut (exemple)
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            // Bordures indestructibles et piliers une case sur deux (style classique)
            if (y == 0 || y == MAP_HEIGHT - 1 || x == 0 || x == MAP_WIDTH - 1 || (x % 2 == 0 && y % 2 == 0))
            {
                game->map[y][x] = TILE_WALL;
            }
            else
            {
                game->map[y][x] = TILE_EMPTY; // Remplir le reste de vide (ou briques plus tard)
            }
        }
    }

    // Désactiver toutes les bombes au départ
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        game->bombs[i].is_active = false;
    }

    // TODO: Charger les ennemis et distribuer les blocs destructibles (TILE_BRICK)
}

/**
 * Gère les entrées de l'utilisateur (ZQSD / Flèches, Espace pour la bombe).
 */
void handle_input(GameState *game)
{
    // Cette partie dépendra de la bibliothèque graphique choisie (SDL2, Raylib, etc.)
    // Exemple logique fictif :
    // if (Touche_Droite_Pressee) déplacer_joueur(game, DIR_RIGHT);
    // if (Touche_Espace_Pressee) poser_bombe(game);
}

/**
 * Met à jour toute la logique interne du jeu à chaque frame.
 */
void update_game(GameState *game)
{
    // 1. Mettre à jour les timers des bombes et déclencher les explosions
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (game->bombs[i].is_active)
        {
            game->bombs[i].timer--;
            if (game->bombs[i].timer <= 0)
            {
                // Fonction à créer : executer_explosion(game, &game->bombs[i]);
                game->bombs[i].is_active = false;
            }
        }
    }

    // 2. Déplacer les ennemis (IA basique)

    // 3. Vérifier les collisions (Joueur touché par ennemi ou explosion)

    // 4. Mettre à jour le timer global du niveau
}

/**
 * S'occupe d'afficher le jeu (le sol, les blocs, le joueur, les animations).
 */
void render_game(const GameState *game)
{
    // 1. Effacer l'écran précédent
    // 2. Dessiner la map (boucle imbriquée sur game->map)
    // 3. Dessiner les bombes actives
    // 4. Dessiner les ennemis
    // 5. Dessiner le joueur s'il est en vie
    // 6. Afficher l'interface (Score, Temps)
}

/**
 * Libère la mémoire ou ferme les fenêtres graphiques proprement.
 */
void clean_game(GameState *game)
{
    // Fermeture des textures, sons et fenêtres
}