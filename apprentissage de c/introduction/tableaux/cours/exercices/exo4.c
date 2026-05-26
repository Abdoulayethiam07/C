#include <stdio.h>
// exercice4
int main()
{
    int indiceMax;
    int indiceMin;
    int max, min;
    int tab[5];
    int i;

    // Lire les 5 entiers depuis l'utilisateur
    printf("Entrez 5 entiers :\n");
    for (i = 0; i < 5; i++)
    {
        printf("Entier %d : ", i + 1);
        scanf("%d", &tab[i]);
    }

    // Initialiser max et min avec le premier élément
    max = min = tab[0];
    indiceMax = indiceMin = 0;

    // Trouver le max et le min
    for (i = 1; i < 5; i++)
    {
        if (tab[i] > max)
        {
            max = tab[i];
            indiceMax = i;
        }
        if (tab[i] < min)
        {
            min = tab[i];
            indiceMin = i;
        }
    }

    // Afficher les résultats
    printf("Le maximum est : %d et son indice est : %d\n", max, indiceMax);
    printf("Le minimum est : %d et son indice est : %d\n", min, indiceMin);

    return 0;
}