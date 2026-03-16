#include <stdio.h>
int main(void)
{
    int i;
    int nombre = 0;
    int c;
    printf("entrez un nombre svp ! \n");
    scanf("%d", &nombre);
    for (i = 0; i < 13; ++i)
    {
        c = nombre * i;
        printf("%d\n", c);
    }
}