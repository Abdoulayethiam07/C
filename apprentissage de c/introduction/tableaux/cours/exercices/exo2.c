#include <stdio.h>
// exercice2
void main()
{
    int tab[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int somme = 0;
    for (int i = 0; i < 10; i++)
    {
        printf("Entrez un nombre svp : ");
        scanf("%d", &tab[i]);
        somme = somme + tab[i];
    }
    printf("La somme des elements du tableau est : %d\n", somme);
}
