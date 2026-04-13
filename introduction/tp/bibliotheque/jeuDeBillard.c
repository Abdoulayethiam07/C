#include <stdio.h>
#include <unistd.h> // Pour la fonction usleep (pause)

int main()
{
    // Dimensions de la table
    int largeur = 40;
    int hauteur = 15;

    // Position et vitesse de la bille (float pour plus de précision)
    float x = 5.0, y = 5.0;
    float vx = 1.2, vy = 0.7;
    float friction = 0.99; // Ralentissement

    printf("--- SIMULATION DE BILLARD ---\n");

    // Boucle de mouvement (s'arrête quand la bille est presque immobile)
    while (vx * vx + vy * vy > 0.01)
    {

        // 1. Mise à jour de la position
        x += vx;
        y += vy;

        // 2. Gestion des rebonds (murs)
        if (x <= 0 || x >= largeur - 1)
            vx *= -1;
        if (y <= 0 || y >= hauteur - 1)
            vy *= -1;

        // 3. Application de la friction
        vx *= friction;
        vy *= friction;

        // 4. Affichage (On "dessine" la table avec des caractères)
        // Effacer l'écran (code spécial pour nettoyer la console)
        printf("\033[H\033[J");

        for (int j = 0; j < hauteur; j++)
        {
            for (int i = 0; i < largeur; i++)
            {
                if (i == (int)x && j == (int)y)
                {
                    printf("O"); // La bille
                }
                else if (i == 0 || i == largeur - 1 || j == 0 || j == hauteur - 1)
                {
                    printf("#"); // Les bords
                }
                else
                {
                    printf(" "); // Vide
                }
            }
            printf("\n");
        }

        usleep(30000); // Pause de 30ms pour voir le mouvement
    }

    printf("\nLa bille s'est arrêtée.\n");
    return 0;
}