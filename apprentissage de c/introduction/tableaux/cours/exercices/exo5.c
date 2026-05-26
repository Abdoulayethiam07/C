#include <stdio.h>
// exercice5
void main()
{
    int compter = 0;
    int tab[10];
    int i;
    printf("saisir un entier svp , il y'en a 10 en tout \n");
    for (i = 0; i < 10; i++)
    {

        scanf("%d ", &tab[i]);
        if (tab[i] == 0)
        {
            compter++;
        }
    }
    printf("le nombre de 0 est : %d", compter);
}