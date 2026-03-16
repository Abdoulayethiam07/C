#include <stdio.h>

int main()
{

    int i;
    int nombre = 0;
    printf("entrez un nombre svp ! \n");
    scanf("%d", &nombre);
    for (i = 0; i < nombre; ++i)
    {
        printf("%d\n", i);
    }
}
