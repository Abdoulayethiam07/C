#include <stdio.h>
#include <stdlib.h>

int main()
{
    int n, valeurAbsolue;

    printf("Entrez un nombre entier (positif ou negatif) : ");
    scanf("%d", &n);

    valeurAbsolue = abs(n);

    printf("La valeur absolue de %d est : %d\n", n, valeurAbsolue);

    return 0;
}
