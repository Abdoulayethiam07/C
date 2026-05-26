#include <stdio.h>
void main()
{
    int max = 0;
    int nombre;
    for (int i = 0; i < 10; i++)
    {
        printf("entrez un nombre svp !\n");
        scanf("%d", &nombre);
        if (nombre > max)
        {
            max = nombre;
        }
        printf("Le nombre maximum est : %d\n", max);
    }
}