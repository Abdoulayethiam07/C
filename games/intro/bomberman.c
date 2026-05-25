/*
 * ============================================================================
 *  BOMBERMAN EN C - CHARPENTE PRINCIPALE
 * ============================================================================
 *
 *  ARCHITECTURE GLOBALE
 *  --------------------
 *  Ce projet suit le pattern "Game Loop" :
 *
 *        ┌─────────────────────────────────────────┐
 *        │              main()                      │
 *        │   init_game()  ──>  démarre tout         │
 *        │                                          │
 *        │   ┌──────────── Game Loop ─────────────┐ │
 *        │   │  handle_input()  → lit le clavier   │ │
 *        │   │  update_game()   → logique/physique │ │
 *        │   │  render_game()   → affichage        │ │
 *        │   └────────────────────────────────────┘ │
 *        │                                          │
 *        │   clean_game()  ──>  libère la mémoire  │
 *        └─────────────────────────────────────────┘
 *
 *  BIBLIOTHÈQUE GRAPHIQUE RECOMMANDÉE : SDL2 ou Raylib
 *  - SDL2  → plus répandue, plus de docs en ligne
 *  - Raylib → plus simple à apprendre, idéale pour débuter
 *
 *  COMPILATION (exemple avec Raylib) :
 *  gcc bomberman.c -o bomberman -lraylib -lm
 *
 *  FICHIERS À CRÉER PAR LA SUITE :
 *  - map.c / map.h         → chargement et manipulation de la carte
 *  - player.c / player.h   → déplacement et actions du joueur
 *  - bomb.c / bomb.h       → logique des bombes et explosions
 *  - enemy.c / enemy.h     → IA des ennemis
 *  - render.c / render.h   → tout ce qui touche à l'affichage
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* ============================================================================
 *  SECTION 1 : CONSTANTES ET CONFIGURATION
 *  Ces valeurs définissent les dimensions du jeu et ses limites.
 *  Les mettre en #define permet de les modifier facilement en un seul endroit.
 * ============================================================================ */

#define MAP_WIDTH 19          /* Nombre de cases en largeur (colonnes) */
#define MAP_HEIGHT 13         /* Nombre de cases en hauteur (lignes)   */
#define TILE_SIZE 48          /* Taille d'une case en pixels (pour le rendu) */
#define MAX_BOMBS 10          /* Bombes simultanées maximum à l'écran  */
#define MAX_ENEMIES 20        /* Ennemis simultanés maximum            */
#define BOMB_FUSE_TIME 180    /* Durée avant explosion en frames (≈3s à 60fps) */
#define EXPLOSION_DURATION 30 /* Durée d'affichage du feu en frames    */
#define PLAYER_START_X 1      /* Position de départ du joueur (en cases) */
#define PLAYER_START_Y 1

/* ============================================================================
 *  SECTION 2 : ÉNUMÉRATIONS
 *  Les enums remplacent des "nombres magiques" par des noms lisibles.
 *  Ex : map[y][x] == TILE_WALL est bien plus clair que map[y][x] == 1
 * ============================================================================ */

/*
 * Types de cases possibles sur la grille.
 * La carte est une matrice 2D de ces valeurs.
 *
 *   [WALL]  = pilier gris indestructible (les colonnes fixes)
 *   [BRICK] = caisse/buisson destructible (peut cacher un power-up)
 *   [EMPTY] = sol, le joueur/ennemi peut y marcher
 *   [EXPLOSION] = case temporairement en feu après une bombe
 */
typedef enum
{
    TILE_EMPTY,
    TILE_WALL,
    TILE_BRICK,
    TILE_EXPLOSION
} TileType;

/*
 * Les 4 directions de déplacement + aucun mouvement.
 * Utilisées aussi bien pour le joueur que pour les ennemis.
 */
typedef enum
{
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT,
    DIR_NONE
} Direction;

/*
 * États possibles du jeu (machine à états).
 * Permet de gérer l'écran titre, le gameplay, le game over, etc.
 * sans avoir de grosses conditions partout dans le code.
 */
typedef enum
{
    STATE_MENU,      /* Écran d'accueil */
    STATE_PLAYING,   /* Partie en cours  */
    STATE_PAUSED,    /* Jeu en pause     */
    STATE_GAME_OVER, /* Le joueur est mort */
    STATE_WIN        /* Niveau terminé (tous les ennemis éliminés) */
} GamePhase;

/* ============================================================================
 *  SECTION 3 : STRUCTURES DE DONNÉES
 *  Ce sont les "objets" du jeu. Chaque struct regroupe les données
 *  qui appartiennent logiquement ensemble.
 * ============================================================================ */

/*
 * Position générique sur la grille (en indices de cases, pas en pixels).
 * Garder la logique en "cases" simplifie les collisions et l'IA.
 * Pour le rendu, on multiplie par TILE_SIZE pour obtenir les pixels.
 */
typedef struct
{
    int x; /* Colonne (0 = gauche) */
    int y; /* Ligne   (0 = haut)   */
} Position;

/* ----------------------------------------------------------
 * JOUEUR (Bomberman lui-même)
 * ---------------------------------------------------------- */
typedef struct
{
    Position pos;        /* Position sur la grille           */
    int lives;           /* Vies restantes (affichées en haut) */
    int bomb_capacity;   /* Combien de bombes il peut avoir sur la carte en même temps */
    int bombs_placed;    /* Combien il en a posé (ne peut pas dépasser bomb_capacity) */
    int explosion_range; /* Portée des explosions en nombre de cases */
    int score;           /* Score cumulé                      */
    bool is_alive;       /* False quand touché → perd une vie */

    /*
     * TODO (ÉTAPE 3 - Power-ups) :
     * Ajouter des booléens comme :
     *   bool has_remote_bomb;   → bombe déclenchée à distance
     *   bool has_kick;          → peut botter une bombe
     *   int  speed_level;       → niveau de vitesse
     */
} Player;

/* ----------------------------------------------------------
 * BOMBE
 * ---------------------------------------------------------- */
typedef struct
{
    Position pos;   /* Case où la bombe a été posée */
    int timer;      /* Compte à rebours avant explosion (en frames) */
    int range;      /* Portée de cette bombe (copiée depuis le joueur au moment du dépôt) */
    bool is_active; /* false = slot disponible dans le tableau bombs[] */
} Bomb;

/* ----------------------------------------------------------
 * EXPLOSION (flamme individuelle)
 * Chaque bombe génère plusieurs "flammes" dans les 4 directions.
 * On les trace sur la map comme TILE_EXPLOSION avec un timer.
 * ---------------------------------------------------------- */
typedef struct
{
    Position pos;
    int duration; /* Frames restantes avant que la flamme s'éteigne */
    bool is_active;
} Explosion;

/* ----------------------------------------------------------
 * ENNEMI
 * ---------------------------------------------------------- */
typedef struct
{
    Position pos;  /* Position sur la grille     */
    Direction dir; /* Direction de déplacement courante */
    bool is_alive;
    int type;       /* 0 = Balloom (lent/bête), 1 = Oneal (plus rapide), etc. */
    int move_timer; /* Compteur pour contrôler la vitesse (bouge tous les N frames) */

    /*
     * TODO (ÉTAPE 4 - IA améliorée) :
     * Ajouter un champ 'strategy' (RANDOM, CHASE, FLEE)
     * pour des comportements différents selon le type.
     */
} Enemy;

/* ----------------------------------------------------------
 * GAMESTATE : Le "cerveau" central du jeu.
 * On passe un pointeur vers cette structure à CHAQUE fonction.
 * C'est l'unique source de vérité sur ce qui se passe dans le jeu.
 * ---------------------------------------------------------- */
typedef struct
{
    TileType map[MAP_HEIGHT][MAP_WIDTH]; /* La grille du niveau (matrice 2D) */

    Player player;                       /* Le joueur (unique instance)      */
    Bomb bombs[MAX_BOMBS];               /* Pool de bombes (slots réutilisables) */
    Explosion explosions[MAX_BOMBS * 5]; /* Pool de flammes (une bombe crée jusqu'à ~5 flammes par rayon) */
    Enemy enemies[MAX_ENEMIES];          /* Tableau des ennemis              */

    int active_enemies_count; /* Ennemis encore en vie (condition de victoire) */
    int game_time;            /* Secondes restantes (le timer affiché en haut)  */
    int frame_counter;        /* Compteur de frames depuis le début de la partie */

    GamePhase phase; /* État courant du jeu (menu, playing, etc.) */
    bool is_running; /* false = quitter la boucle principale */
} GameState;

/* ============================================================================
 *  SECTION 4 : PROTOTYPES
 *  Déclarer les fonctions ici permet de les implémenter dans n'importe quel
 *  ordre et facilite la séparation future en plusieurs fichiers .c/.h
 * ============================================================================ */

/* --- Cycle de vie principal --- */
void init_game(GameState *game);
void handle_input(GameState *game);
void update_game(GameState *game);
void render_game(const GameState *game);
void clean_game(GameState *game);

/* --- Logique Carte --- */
void generate_map(GameState *game);
bool is_walkable(const GameState *game, int x, int y);

/* --- Logique Joueur --- */
void move_player(GameState *game, Direction dir);
void place_bomb(GameState *game);

/* --- Logique Bombes & Explosions --- */
void update_bombs(GameState *game);
void trigger_explosion(GameState *game, Bomb *bomb);
void update_explosions(GameState *game);

/* --- Logique Ennemis --- */
void update_enemies(GameState *game);
void move_enemy(GameState *game, Enemy *enemy);

/* --- Vérifications --- */
void check_player_death(GameState *game);
void check_enemy_deaths(GameState *game);
void check_win_condition(GameState *game);

/* ============================================================================
 *  SECTION 5 : POINT D'ENTRÉE
 * ============================================================================ */
int main(void)
{
    GameState game;

    /*
     * ÉTAPE 1 : Initialisation
     * Met à zéro et configure toutes les structures.
     */
    init_game(&game);

    /*
     * ÉTAPE 2 : La Game Loop
     *
     * Elle tourne à environ 60 fois par seconde (si tu utilises
     * SDL2 avec SDL_Delay ou Raylib avec SetTargetFPS).
     *
     * L'ordre est CRUCIAL :
     *   1. Lire les inputs  (ce que l'utilisateur a demandé)
     *   2. Mettre à jour    (calculer les conséquences)
     *   3. Afficher         (montrer le résultat)
     */
    while (game.is_running)
    {
        handle_input(&game);
        update_game(&game);
        render_game(&game);
    }

    /*
     * ÉTAPE 3 : Nettoyage
     * Libérer la mémoire allouée dynamiquement, fermer les fenêtres.
     */
    clean_game(&game);

    return EXIT_SUCCESS;
}

/* ============================================================================
 *  SECTION 6 : IMPLÉMENTATION DES FONCTIONS
 * ============================================================================ */

/* ----------------------------------------------------------------------------
 *  init_game()
 *  Remet le jeu à zéro et prépare le premier niveau.
 *  Doit être appelée aussi lors d'd'un "Retry" (mort du joueur).
 * ---------------------------------------------------------------------------- */
void init_game(GameState *game)
{
    /* Paramètres globaux */
    game->is_running = true;
    game->phase = STATE_PLAYING; /* TODO : changer en STATE_MENU plus tard */
    game->game_time = 195;
    game->frame_counter = 0;
    game->active_enemies_count = 0;

    /* Configuration du joueur */
    game->player.pos.x = PLAYER_START_X;
    game->player.pos.y = PLAYER_START_Y;
    game->player.lives = 3;
    game->player.bomb_capacity = 1;
    game->player.bombs_placed = 0;
    game->player.explosion_range = 2;
    game->player.score = 0;
    game->player.is_alive = true;

    /* Génération de la carte */
    generate_map(game);

    /* Désactivation de tous les slots de bombes/explosions */
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        game->bombs[i].is_active = false;
    }
    for (int i = 0; i < MAX_BOMBS * 5; i++)
    {
        game->explosions[i].is_active = false;
    }

    /*
     * TODO (ÉTAPE 1) : Placer les ennemis sur la map.
     * Exemple :
     *   game->enemies[0].pos.x = 17;
     *   game->enemies[0].pos.y = 11;
     *   game->enemies[0].is_alive = true;
     *   game->enemies[0].type = 0;
     *   game->active_enemies_count++;
     */
}

/* ----------------------------------------------------------------------------
 *  generate_map()
 *  Construit la grille du niveau.
 *  Règle classique Bomberman :
 *   - Bordures et cases paires/paires → WALL (indestructible)
 *   - Reste → EMPTY ou BRICK (destructible, à distribuer aléatoirement)
 *  Les 3 premières cases autour du joueur doivent rester EMPTY
 *  pour qu'il ne soit pas coincé au départ.
 * ---------------------------------------------------------------------------- */
void generate_map(GameState *game)
{
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {

            /* Piliers fixes : bordures ET colonnes/lignes de coordonnées paires */
            if (y == 0 || y == MAP_HEIGHT - 1 ||
                x == 0 || x == MAP_WIDTH - 1 ||
                (x % 2 == 0 && y % 2 == 0))
            {
                game->map[y][x] = TILE_WALL;
            }
            else
            {
                /*
                 * TODO (ÉTAPE 1) : Remplacer ceci par une distribution aléatoire.
                 * Utiliser rand() % 100 < 70 pour mettre 70% de briques.
                 * Protéger la zone de départ du joueur :
                 *   if (x == 1 && y == 1) → EMPTY
                 *   if (x == 2 && y == 1) → EMPTY
                 *   if (x == 1 && y == 2) → EMPTY
                 */
                game->map[y][x] = TILE_EMPTY;
            }
        }
    }
}

/* ----------------------------------------------------------------------------
 *  is_walkable()
 *  Renvoie true si la case (x,y) est accessible (pas de mur ni de brique).
 *  Utilisée partout : déplacement joueur, déplacement ennemis.
 * ---------------------------------------------------------------------------- */
bool is_walkable(const GameState *game, int x, int y)
{
    /* Sécurité : refuser les cases hors grille */
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return false;

    TileType tile = game->map[y][x];
    return (tile == TILE_EMPTY || tile == TILE_EXPLOSION);
    /*
     * Note : TILE_EXPLOSION est "walkable" dans le sens où on peut
     * techniquement y entrer, mais check_player_death() tuera le joueur.
     */
}

/* ----------------------------------------------------------------------------
 *  handle_input()
 *  Lit les entrées clavier et déclenche les actions correspondantes.
 *
 *  TODO (ÉTAPE 2) : Brancher avec SDL2 ou Raylib.
 *
 *  Avec SDL2 :
 *    const Uint8 *keys = SDL_GetKeyboardState(NULL);
 *    if (keys[SDL_SCANCODE_UP])    move_player(game, DIR_UP);
 *    if (keys[SDL_SCANCODE_SPACE]) place_bomb(game);
 *
 *  Avec Raylib :
 *    if (IsKeyDown(KEY_UP))    move_player(game, DIR_UP);
 *    if (IsKeyPressed(KEY_SPACE)) place_bomb(game);
 *
 *  ATTENTION : IsKeyDown = maintenu enfoncé (mouvement fluide)
 *              IsKeyPressed = une seule fois par appui (pose de bombe)
 * ---------------------------------------------------------------------------- */
void handle_input(GameState *game)
{
    /* Quitter le jeu */
    /* if (IsKeyPressed(KEY_ESCAPE)) game->is_running = false; */

    /* Déplacement joueur */
    /* if (IsKeyDown(KEY_UP))    move_player(game, DIR_UP);    */
    /* if (IsKeyDown(KEY_DOWN))  move_player(game, DIR_DOWN);  */
    /* if (IsKeyDown(KEY_LEFT))  move_player(game, DIR_LEFT);  */
    /* if (IsKeyDown(KEY_RIGHT)) move_player(game, DIR_RIGHT); */

    /* Pose de bombe */
    /* if (IsKeyPressed(KEY_SPACE)) place_bomb(game); */
}

/* ----------------------------------------------------------------------------
 *  move_player()
 *  Tente de déplacer le joueur dans la direction donnée.
 *  Ne bouge que si la case cible est libre.
 * ---------------------------------------------------------------------------- */
void move_player(GameState *game, Direction dir)
{
    int new_x = game->player.pos.x;
    int new_y = game->player.pos.y;

    /* Calculer la case cible selon la direction */
    switch (dir)
    {
    case DIR_UP:
        new_y--;
        break;
    case DIR_DOWN:
        new_y++;
        break;
    case DIR_LEFT:
        new_x--;
        break;
    case DIR_RIGHT:
        new_x++;
        break;
    default:
        return;
    }

    /* Valider avant de déplacer : pas de mur, pas de brique */
    if (is_walkable(game, new_x, new_y))
    {
        game->player.pos.x = new_x;
        game->player.pos.y = new_y;
    }

    /*
     * TODO (ÉTAPE 3 - Mouvement fluide en pixels) :
     * Actuellement le joueur "saute" de case en case.
     * Pour un mouvement pixel par pixel, ajouter dans Player :
     *   float pixel_x, pixel_y;  → position en pixels
     *   float speed;             → vitesse en pixels/frame
     * Et interpoler vers la case cible progressivement.
     */
}

/* ----------------------------------------------------------------------------
 *  place_bomb()
 *  Pose une bombe sur la case actuelle du joueur, si possible.
 *  Cherche un slot libre dans le tableau bombs[].
 * ---------------------------------------------------------------------------- */
void place_bomb(GameState *game)
{
    /* Vérifier que le joueur n'a pas atteint sa capacité de bombes */
    if (game->player.bombs_placed >= game->player.bomb_capacity)
        return;

    /* Trouver un slot de bombe inactif */
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!game->bombs[i].is_active)
        {
            game->bombs[i].pos.x = game->player.pos.x;
            game->bombs[i].pos.y = game->player.pos.y;
            game->bombs[i].timer = BOMB_FUSE_TIME;
            game->bombs[i].range = game->player.explosion_range;
            game->bombs[i].is_active = true;

            game->player.bombs_placed++;
            return; /* Un seul slot occupé par appel */
        }
    }
}

/* ----------------------------------------------------------------------------
 *  update_bombs()
 *  Décrémente le timer de chaque bombe active.
 *  Déclenche l'explosion quand le timer arrive à 0.
 * ---------------------------------------------------------------------------- */
void update_bombs(GameState *game)
{
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (!game->bombs[i].is_active)
            continue;

        game->bombs[i].timer--;

        if (game->bombs[i].timer <= 0)
        {
            trigger_explosion(game, &game->bombs[i]);
            game->bombs[i].is_active = false;
            game->player.bombs_placed--; /* Le slot est libéré */
        }
    }
}

/* ----------------------------------------------------------------------------
 *  trigger_explosion()
 *  Propage le feu depuis la bombe dans les 4 directions.
 *
 *  Algorithme :
 *  Pour chaque direction, avancer case par case jusqu'à :
 *    - Atteindre la portée max (bomb->range)
 *    - Toucher un WALL   → stopper sans détruire
 *    - Toucher un BRICK  → détruire la brique et stopper
 *    - Toucher EMPTY     → marquer TILE_EXPLOSION, continuer
 *
 *  TODO (ÉTAPE 2) : Implémenter la propagation.
 * ---------------------------------------------------------------------------- */
void trigger_explosion(GameState *game, Bomb *bomb)
{
    /* La case de la bombe elle-même explose toujours */
    game->map[bomb->pos.y][bomb->pos.x] = TILE_EXPLOSION;

    /*
     * TODO : Propager dans les 4 directions.
     *
     * int dx[] = { 0,  0, -1, 1 };
     * int dy[] = {-1,  1,  0, 0 };
     *
     * for (int d = 0; d < 4; d++) {
     *     for (int step = 1; step <= bomb->range; step++) {
     *         int ex = bomb->pos.x + dx[d] * step;
     *         int ey = bomb->pos.y + dy[d] * step;
     *
     *         if (ex < 0 || ex >= MAP_WIDTH || ey < 0 || ey >= MAP_HEIGHT) break;
     *         if (game->map[ey][ex] == TILE_WALL)  break;
     *         if (game->map[ey][ex] == TILE_BRICK) {
     *             game->map[ey][ex] = TILE_EXPLOSION;
     *             break; // La flamme ne traverse pas les briques
     *         }
     *         game->map[ey][ex] = TILE_EXPLOSION;
     *     }
     * }
     */
}

/* ----------------------------------------------------------------------------
 *  update_explosions()
 *  Efface les flammes après EXPLOSION_DURATION frames.
 *
 *  TODO (ÉTAPE 2) :
 *  Parcourir la map, trouver les TILE_EXPLOSION et décrémenter
 *  un compteur associé. Quand il atteint 0 → remettre TILE_EMPTY.
 *  (Pour cela, utiliser le tableau explosions[] de GameState)
 * ---------------------------------------------------------------------------- */
void update_explosions(GameState *game)
{
    /* TODO */
    (void)game; /* Évite le warning "paramètre non utilisé" en attendant */
}

/* ----------------------------------------------------------------------------
 *  update_enemies()
 *  Déplace tous les ennemis en vie.
 *  Appelée à chaque frame depuis update_game().
 * ---------------------------------------------------------------------------- */
void update_enemies(GameState *game)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!game->enemies[i].is_alive)
            continue;

        /* Décrémente le timer de mouvement pour contrôler la vitesse */
        game->enemies[i].move_timer--;
        if (game->enemies[i].move_timer <= 0)
        {
            move_enemy(game, &game->enemies[i]);

            /* Réinitialise le délai (type 0 = lent, type 1 = rapide) */
            game->enemies[i].move_timer = (game->enemies[i].type == 0) ? 30 : 15;
        }
    }
}

/* ----------------------------------------------------------------------------
 *  move_enemy()
 *  Déplace un ennemi selon son comportement (IA basique).
 *  Stratégie "Balloom" (type 0) : avance tout droit, tourne si mur.
 *
 *  TODO (ÉTAPE 4) : Ajouter une IA de poursuite pour les ennemis de type 1+.
 *  Algorithme BFS/A* pour trouver le chemin vers le joueur.
 * ---------------------------------------------------------------------------- */
void move_enemy(GameState *game, Enemy *enemy)
{
    int dx[] = {0, 0, -1, 1}; /* Correspondance avec DIR_UP, DOWN, LEFT, RIGHT */
    int dy[] = {-1, 1, 0, 0};

    int d = enemy->dir;
    int new_x = enemy->pos.x + dx[d];
    int new_y = enemy->pos.y + dy[d];

    if (is_walkable(game, new_x, new_y))
    {
        /* Continuer dans la même direction */
        enemy->pos.x = new_x;
        enemy->pos.y = new_y;
    }
    else
    {
        /*
         * Case bloquée : choisir une nouvelle direction aléatoire.
         * TODO : Améliorer avec un choix parmi les directions libres.
         */
        enemy->dir = (Direction)(rand() % 4);
    }
}

/* ----------------------------------------------------------------------------
 *  check_player_death()
 *  Vérifie si le joueur se trouve sur une case TILE_EXPLOSION
 *  ou sur la même case qu'un ennemi.
 * ---------------------------------------------------------------------------- */
void check_player_death(GameState *game)
{
    if (!game->player.is_alive)
        return;

    int px = game->player.pos.x;
    int py = game->player.pos.y;

    /* Contact avec une flamme */
    if (game->map[py][px] == TILE_EXPLOSION)
    {
        game->player.is_alive = false;
        game->player.lives--;

        if (game->player.lives <= 0)
            game->phase = STATE_GAME_OVER;
        else
        {
            /*
             * TODO (ÉTAPE 3) : Respawn du joueur.
             * Remettre le joueur à sa position de départ,
             * lui accorder une invincibilité temporaire (clignotement).
             */
        }
    }

    /* Contact avec un ennemi */
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].is_alive &&
            game->enemies[i].pos.x == px &&
            game->enemies[i].pos.y == py)
        {
            game->player.is_alive = false;
            game->player.lives--;

            if (game->player.lives <= 0)
                game->phase = STATE_GAME_OVER;
        }
    }
}

/* ----------------------------------------------------------------------------
 *  check_enemy_deaths()
 *  Parcourt les ennemis et tue ceux qui se trouvent sur une flamme.
 *  Ajoute des points au score du joueur pour chaque ennemi tué.
 * ---------------------------------------------------------------------------- */
void check_enemy_deaths(GameState *game)
{
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (!game->enemies[i].is_alive)
            continue;

        int ex = game->enemies[i].pos.x;
        int ey = game->enemies[i].pos.y;

        if (game->map[ey][ex] == TILE_EXPLOSION)
        {
            game->enemies[i].is_alive = false;
            game->active_enemies_count--;
            game->player.score += 100; /* TODO : Ajuster selon le type d'ennemi */
        }
    }
}

/* ----------------------------------------------------------------------------
 *  check_win_condition()
 *  Le joueur gagne quand tous les ennemis sont éliminés.
 * ---------------------------------------------------------------------------- */
void check_win_condition(GameState *game)
{
    if (game->active_enemies_count <= 0 && game->phase == STATE_PLAYING)
    {
        game->phase = STATE_WIN;
        /*
         * TODO (ÉTAPE 5) :
         * - Afficher un écran de victoire
         * - Charger le niveau suivant après un délai
         */
    }
}

/* ----------------------------------------------------------------------------
 *  update_game()
 *  Orchestre toutes les mises à jour logiques à chaque frame.
 *  C'est le "chef d'orchestre" : il appelle toutes les sous-fonctions
 *  dans le bon ordre.
 * ---------------------------------------------------------------------------- */
void update_game(GameState *game)
{
    if (game->phase != STATE_PLAYING)
        return;

    game->frame_counter++;

    /* Décrémenter le timer global toutes les 60 frames (≈ 1 seconde) */
    if (game->frame_counter % 60 == 0)
    {
        game->game_time--;
        if (game->game_time <= 0)
        {
            game->phase = STATE_GAME_OVER; /* Temps écoulé */
        }
    }

    /* Mise à jour des bombes (timers + explosions) */
    update_bombs(game);

    /* Mise à jour des durées des flammes */
    update_explosions(game);

    /* Déplacement des ennemis selon leur IA */
    update_enemies(game);

    /* Vérification des collisions mortelles */
    check_player_death(game);
    check_enemy_deaths(game);

    /* Condition de victoire */
    check_win_condition(game);
}

/* ----------------------------------------------------------------------------
 *  render_game()
 *  Dessine le jeu à l'écran à chaque frame.
 *
 *  TODO (ÉTAPE 2) : Remplacer les commentaires par de vrais appels graphiques.
 *
 *  Ordre d'affichage (important pour les couches) :
 *    1. Fond (sol)
 *    2. Carte (murs, briques)
 *    3. Flammes
 *    4. Bombes
 *    5. Ennemis
 *    6. Joueur
 *    7. HUD (score, temps, vies) → toujours au premier plan
 * ---------------------------------------------------------------------------- */
void render_game(const GameState *game)
{
    /* --- 1. Effacer l'écran --- */
    /* BeginDrawing(); ClearBackground(BLACK); */

    /* --- 2. Dessiner la carte case par case --- */
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            int pixel_x = x * TILE_SIZE;
            int pixel_y = y * TILE_SIZE;

            switch (game->map[y][x])
            {
            case TILE_WALL:
                /* DrawRectangle(pixel_x, pixel_y, TILE_SIZE, TILE_SIZE, GRAY); */
                break;
            case TILE_BRICK:
                /* DrawTexture(tex_brick, pixel_x, pixel_y, WHITE); */
                break;
            case TILE_EXPLOSION:
                /* DrawTexture(tex_fire, pixel_x, pixel_y, WHITE); */
                break;
            default: /* TILE_EMPTY */
                /* DrawTexture(tex_floor, pixel_x, pixel_y, WHITE); */
                break;
            }
            (void)pixel_x;
            (void)pixel_y; /* Évite les warnings en attendant */
        }
    }

    /* --- 3. Dessiner les bombes actives --- */
    for (int i = 0; i < MAX_BOMBS; i++)
    {
        if (game->bombs[i].is_active)
        {
            /* DrawTexture(tex_bomb,
             *   game->bombs[i].pos.x * TILE_SIZE,
             *   game->bombs[i].pos.y * TILE_SIZE, WHITE); */
        }
    }

    /* --- 4. Dessiner les ennemis --- */
    for (int i = 0; i < MAX_ENEMIES; i++)
    {
        if (game->enemies[i].is_alive)
        {
            /* DrawTexture(tex_enemy,
             *   game->enemies[i].pos.x * TILE_SIZE,
             *   game->enemies[i].pos.y * TILE_SIZE, WHITE); */
        }
    }

    /* --- 5. Dessiner le joueur --- */
    if (game->player.is_alive)
    {
        /* DrawTexture(tex_player,
         *   game->player.pos.x * TILE_SIZE,
         *   game->player.pos.y * TILE_SIZE, WHITE); */
    }

    /* --- 6. HUD (Score, Temps, Vies) --- */
    /*
     * DrawText("SCORE", 10, 10, 20, WHITE);
     * DrawText(TextFormat("%05d", game->player.score), 10, 30, 20, YELLOW);
     * DrawText(TextFormat("TIME %03d", game->game_time), 400, 10, 20, WHITE);
     * DrawText(TextFormat("LEFT %d", game->player.lives), 700, 10, 20, WHITE);
     */

    /* --- 7. Fin de frame --- */
    /* EndDrawing(); */
}

/* ----------------------------------------------------------------------------
 *  clean_game()
 *  Libère toutes les ressources allouées dynamiquement.
 *
 *  TODO (ÉTAPE 2) : Décharger les textures, fermer la fenêtre.
 *  Exemple Raylib :
 *    UnloadTexture(tex_player);
 *    UnloadTexture(tex_bomb);
 *    CloseWindow();
 * ---------------------------------------------------------------------------- */
void clean_game(GameState *game)
{
    (void)game; /* Rien à libérer pour l'instant */
}

/*
 * =============================================================================
 *  FEUILLE DE ROUTE - PROCHAINES ÉTAPES
 * =============================================================================
 *
 *  ÉTAPE 1 : Map et affichage console (sans bibliothèque graphique)
 *  ----------------------------------------------------------------
 *  - Compléter generate_map() avec une distribution aléatoire de TILE_BRICK
 *  - Écrire une render_game() temporaire qui affiche la grille avec des
 *    caractères ASCII dans le terminal (# = mur, B = brique, P = joueur, etc.)
 *  - Tester la génération et valider que la logique est correcte
 *
 *  ÉTAPE 2 : Intégration graphique (Raylib recommandé)
 *  ---------------------------------------------------
 *  - Installer Raylib : https://github.com/raysan5/raylib
 *  - InitWindow() dans init_game(), SetTargetFPS(60)
 *  - Charger les textures avec LoadTexture()
 *  - Brancher handle_input() avec IsKeyDown() / IsKeyPressed()
 *  - Remplir render_game() avec DrawTexture()
 *
 *  ÉTAPE 3 : Joueur fonctionnel
 *  ----------------------------
 *  - Implémenter le mouvement fluide en pixels (interpolation)
 *  - Compléter place_bomb() et trigger_explosion() avec la propagation
 *  - Compléter update_explosions() pour éteindre le feu après N frames
 *  - Ajouter l'animation du joueur (spritesheet : marche, mort)
 *
 *  ÉTAPE 4 : Ennemis et IA
 *  -----------------------
 *  - Spawn des ennemis à des positions valides sur la map
 *  - Valider le comportement aléatoire "Balloom"
 *  - Ajouter un comportement de poursuite pour les ennemis avancés
 *    (algorithme BFS sur la grille)
 *
 *  ÉTAPE 5 : Progression et polish
 *  --------------------------------
 *  - Implémenter les power-ups (cachés sous TILE_BRICK détruite)
 *    → Flame+ (augmente explosion_range)
 *    → Bomb+  (augmente bomb_capacity)
 *    → Speed+ (augmente la vitesse du joueur)
 *  - Ajouter plusieurs niveaux (augmenter nb ennemis / briques)
 *  - Écran titre, écran Game Over, high score
 *  - Sons et musique (Raylib : LoadSound(), PlaySound())
 *
 * =============================================================================
 */
