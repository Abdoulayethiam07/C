#include <stdio.h>
// exercice3
void main()
{
    int maxi;
    int indice;
    int tab[5] = {1, 2, 3, 4, 5};
    int i;
    maxi = tab[0];
    indice = 0;
    for (i = 1; i < 5; i++)
    {
        if (tab[i] > maxi)
        {
            maxi = tab[i];
            indice = i;
        }
    }
    printf("le maximum de ces nombres est : %d\n", maxi);
    printf("l'indice de ce nombre est : %d\n", indice);
}