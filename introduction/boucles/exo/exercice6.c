#include <stdio.h>
void main()
{
    int max = 0;
    int min = 0;
    for (int i = 0; i < 10; i++)
    {
        int nombre;
        printf("Entrez un nombre svp ! \n");
        scanf("%d", &nombre);
        if (nombre > max)
        {
            max = nombre;
        }
        if (i == 0)
        {
            min = nombre;
        }
        else if (nombre < min)
        {
            min = nombre;
        }
    }
    printf("Le plus grand nombre est : %d\n", max);
    printf("Le plus petit nombre est : %d\n", min);
}